#ifndef MAIN_ARDUINO_H
#define MAIN_ARDUINO_H

// Include headers
//#include <math.h>
#include "date_time.h"
#include "sensor_record.h"
#include "timer.h"
#include "interconnect.h"
#include "state_flags.h"
#include "settings.h"
#include "bluetooth_interconnect.h"

// Current controller settings and status
Settings settings;

// Sensor record buffer
constexpr int           sensorRecordMax = 8;
int                     sensorRecordCount = 0;
SensorRecord            sensorRecord[sensorRecordMax];
SensorRecord::IndexType sensorRecordIndex = 1;

// Controller timers
// The constructor parameter takes milliseconds, hence the *1000 to convert to seconds
Timer timerReadSensors(5 * 1000);
Timer timerSendToDatabase(2 * 1000);

// A synchronisation-key used when requesting to send data to the database
char sendToDatabaseSyncKey = '0';

// Interconnect buffers
// Used for managing data transfer between different components, such as the Wifi or Bluetooth modules
Interconnect          linkWifi(Serial3, INTERCONNECT_BUFFER_ARDUINO_TO_ESP8266,   INTERCONNECT_BUFFER_ESP8266_TO_ARDUINO);
Interconnect          linkPC(Serial,    INTERCONNECT_BUFFER_ARDUINO_TO_PC,        INTERCONNECT_BUFFER_PC_TO_ARDUINO);
BluetoothInterconnect linkBT(Serial1,   INTERCONNECT_BUFFER_ARDUINO_TO_BLUETOOTH, INTERCONNECT_BUFFER_BLUETOOTH_TO_ARDUINO);

// A list of interconnects
// Used when checking who sent a message
enum class LinkId {
	None,
	Wifi,
	PC,
	BT
};

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
void setup() {

	// Setup serial connections
	Serial.begin(SERIAL_BITRATE_ARDUINO_PC); // For PC connection
	Serial3.begin(SERIAL_BITRATE_ARDUINO_ESP8266); // For ESP8266 connection
	Serial1.begin(9600); // For Bluetooth module

	// Initialise settings data
	settings.init();

	// Debug message
	//Serial.println(F("Arduino starting..."));
}

// Runs repeatedly once the Arduino has finished initialisation
// This function will only run after setup() has completed
void loop() {

	// Get current time in milliseconds
	// This value will overflow (go back to zero) roughly every 50 days
	// It will also reset (go back to zero) if the Arduino is powered off
	unsigned long currentTime = millis();

	// Check if the sensors need to be read
	// Also check if there is enough buffer space to make a new sensor record
	if (timerReadSensors.triggered(currentTime)) {
		if ((sensorRecordCount < sensorRecordMax)) {
			sensorRecord[sensorRecordCount].readAll();
			sensorRecord[sensorRecordCount].index = sensorRecordIndex++;
			//sensorRecord[sensorRecordCount].dateTime = currentTime; // TODO
			sensorRecordCount++;
		}
	}

	// Check if it is time to send data to the database
	// Also check if there is actually any data to send
	if (timerSendToDatabase.triggered(currentTime)) {
		if ((sensorRecordCount > 0) || !settings.isServerInformed()) {
			linkWifi.send(Interconnect::SendToDatabaseRequest, String(sendToDatabaseSyncKey));
		}
	}

	// Update interconnects
	linkWifi.update();
	linkPC.update();
	linkBT.update();

	// Try to receive any pending messages
	LinkId sender = LinkId::None;
	Interconnect::Type header = Interconnect::None;
	String payload;
	if (linkWifi.receive(header, payload)) sender = LinkId::Wifi;
	else if (linkPC.receive(header, payload)) sender = LinkId::PC;
	else if (linkBT.receive(header, payload)) sender = LinkId::BT;

	// Process any received message
	if (sender != LinkId::None) {
		//Serial.print("header: '");
		//Serial.print((char)header);
		//Serial.print("', payload: ");
		//Serial.println(payload);
		switch (header) {

			case Interconnect::SendToDatabaseAllow:
				if (sender == LinkId::Wifi) {

					// Check the synchronisation key
					// If not as expected then this might be an old acknowledgement
					if (sendToDatabaseSyncKey == payload[0]) {

						// Increment synchronisation key for next time
						sendToDatabaseSyncKey = '0' + ((sendToDatabaseSyncKey + 1 - '0') % ('Z' - '0'));

						// Make sure there is actually data to send
						if ((sensorRecordCount > 0) || !settings.isServerInformed()) {

							// Generate JSON payload
							// This will contain the controller status and/or a list of sensor records
							payload = F("{\"Guid\":\"");
							payload += settings.systemGuid();
							payload += F("\",\"Status\":");
							if (!settings.isServerInformed()) {
								settings.toJson(payload);
								settings.setServerInformed();
							} else {
								payload += F("null");
							}
							payload += F(",\"Records\":[");
							while (sensorRecordCount > 0) {
								sensorRecord[--sensorRecordCount].toJson(payload);
								if (sensorRecordCount) payload += ',';
							}
							payload += F("]}");

							// Send data to ESP8266, so that it can forward it to the database
							linkWifi.sendForce(Interconnect::SendToDatabase, payload);
						}
					}
				}
				break;

			case Interconnect::SendToDatabaseSuccess:
				if (sender == LinkId::Wifi) {

					// Received notification that the last SendToDatabase was successful
					// Any reply may contain new settings for the server, so check for that
					if (payload.length() > 2) {
						settings.fromJson(payload);
					}
				}
				break;

			case Interconnect::SendToDatabaseFailure:
				if (sender == LinkId::Wifi) {

					// TODO: Improve this. Add better error handling or something
					linkPC.sendForce(Interconnect::GeneralNotification, String(F("SendToDatabaseFailure: ")) + payload);
				}
				break;

			case Interconnect::GeneralNotification:

				// Received a status message from the ESP8266 which should be passed to the serial port for debugging
				linkPC.sendForce(Interconnect::GeneralNotification, String(F("ESP8266: ")) + payload);
				break;

			case Interconnect::EchoArduino:

				// Received an echo request from the serial port (used for debugging)
				// Return the same message
				linkPC.sendForce(Interconnect::GeneralNotification, String(F("Arduino: Echo: ")) + payload);
				break;

			case Interconnect::EchoESP8266:

				// Received an echo request from the serial port, directed toward the ESP8266 (used for debugging)
				// Try to forward the request to the ESP8266
				if (!linkWifi.send(header, payload)) {
					linkPC.sendForce(Interconnect::GeneralNotification, F("Arduino: Failed to echo to ESP8266"));
				}
				break;

			case Interconnect::SetSettings:

				// Received new settings
				if (payload.length() > 2) {
					payload = settings.fromJson(payload) ? F("Ok") : F("Error");

					// Send reply
					switch (sender) {
						case LinkId::PC: linkPC.sendForce(header, payload); break;
						case LinkId::BT: linkBT.sendForce(header, payload); break;
					}
				}
				break;

			case Interconnect::GetSettings:

				// Settings data was requested
				switch (sender) {
					case LinkId::PC: linkPC.sendForce(header, settings.toJson(true)); break;
					case LinkId::BT: linkBT.sendForce(header, settings.toJson(true)); break;
				}
				break;

			case Interconnect::SetGuid:

				// Save new GUID
				// Send reply
				settings.systemGuid(payload);
				settings.save();
				payload = F("Ok");
				switch (sender) {
					case LinkId::PC: linkPC.sendForce(header, payload); break;
					case LinkId::BT: linkBT.sendForce(header, payload); break;
				}
				break;

			case Interconnect::GetGuid:

				// GUID was requested
				switch (sender) {
					case LinkId::PC: linkPC.sendForce(header, settings.systemGuid()); break;
					case LinkId::BT: linkBT.sendForce(header, settings.systemGuid()); break;
				}
				break;

			case Interconnect::DebugSendToServerKeepHeaders:
			case Interconnect::DebugSendToServer:
			case Interconnect::SetServerAddress:
			case Interconnect::GetServerAddress:

				// Received a command which needs to be forwarded to the ESP8266
				if (sender != LinkId::Wifi) {
					if (!linkWifi.send(header, payload)) {
						linkPC.sendForce(Interconnect::GeneralNotification, F("Arduino: Interconnect buffer is full"));
					}
				}
				break;

			default:
				break;
		}
	}
}

#endif
