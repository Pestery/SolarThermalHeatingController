#ifndef MAIN_ARDUINO_H
#define MAIN_ARDUINO_H

// Include headers
//#include <math.h>
#include "date_time.h"
#include "sensor_record.h"
#include "timer.h"
#include "interconnect.h"
#include "state_flags.h"

// State flags
#define FLAG_PENDING_SENSOR_RECORD     (StateFlags::Type(1 << 0))
#define FLAG_PENDING_CURRENT_STATUS    (StateFlags::Type(1 << 1))
#define FLAG_PENDING_DATA_FOR_DATABASE (FLAG_PENDING_SENSOR_RECORD | FLAG_PENDING_CURRENT_STATUS)

// Declare global values
StateFlags   stateFlags;
SensorRecord sensorRecord;
Interconnect linkWifi(Serial3, INTERCONNECT_BUFFER_ARDUINO_TO_ESP8266, INTERCONNECT_BUFFER_ESP8266_TO_ARDUINO);
Interconnect linkPC(Serial,    INTERCONNECT_BUFFER_ARDUINO_TO_PC,      INTERCONNECT_BUFFER_PC_TO_ARDUINO);
Timer        timerReadSensors(5 * 1000);
Timer        timerCheckForCommands(60 * 1000);
Timer        timerLargeMessageReminder(2500);
char         sendLargeMessageSyncKey = '0';

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
void setup() {

	// Setup serial connections
	Serial.begin(SERIAL_BITRATE_ARDUINO_PC); // For PC connection
	Serial3.begin(SERIAL_BITRATE_ARDUINO_ESP8266); // For ESP8266 connection

	// Debug message
	//Serial.println(F("Arduino starting..."));
}

// Runs repeatedly once the Arduino has finished initialisation
// This function will only run after setup() has completed
void loop() {

	// Update interconnects
	linkWifi.update();
	linkPC.update();

	// Check for received messages
	Interconnect::Type header = Interconnect::None;
	String payload;
	if (linkWifi.receive(header, payload) || linkPC.receive(header, payload)) {

		// Process received message
		switch (header) {

			case Interconnect::AllowSendDataForDatabase:

				// Check the synchronisation key
				// If not as expected then this might be an old acknowledgement
				if (sendLargeMessageSyncKey == payload[0]) {

					// Increment synchronisation key for next time
					sendLargeMessageSyncKey = '0' + ((sendLargeMessageSyncKey + 1 - '0') % ('Z' - '0'));

					// Allowed to send data for database to the ESP8266
					if (stateFlags.anySet(FLAG_PENDING_SENSOR_RECORD)) {
						if (linkWifi.send(Interconnect::DataForDatabase, sensorRecord.toJSON())) {
							stateFlags.reset(FLAG_PENDING_SENSOR_RECORD);
						} else {
							Serial.println(F("Failed to add sensor data to Interconnect"));
						}
					} else if (stateFlags.anySet(FLAG_PENDING_CURRENT_STATUS)) {
						// TODO: Add code to send current controller status
						stateFlags.reset(FLAG_PENDING_CURRENT_STATUS);
					}

					// Now that data has been added to the send queue, reset the large data timer
					timerLargeMessageReminder.reset(millis());
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
					linkPC.sendForce(Interconnect::GeneralNotification, F("Arduino: Echo: Failed to send echo request to ESP8266. Buffer full."));
				}
				break;

			default:
				break;
		}
	}

	// Get current time in milliseconds
	// This value will overflow (go back to zero) roughly every 50 days
	// It will also reset (go back to zero) if the Arduino is powered off
	unsigned long currentTime = millis();

	// Check if time to read sensors
	if (timerReadSensors.triggered(currentTime)) {
		sensorRecord.readAll();
		stateFlags.set(FLAG_PENDING_SENSOR_RECORD);
		//Serial.println(F("Read sensors"));
	}

	// Check if any data needs to be sent to the database, via the ESP8266
	if (stateFlags.anySet(FLAG_PENDING_DATA_FOR_DATABASE)) {
		if (linkWifi.emptySendBuffer() && timerLargeMessageReminder.triggered(currentTime)) {
			linkWifi.send(Interconnect::RequestSendDataForDatabase, String(sendLargeMessageSyncKey));
		}
	}
}

#endif
