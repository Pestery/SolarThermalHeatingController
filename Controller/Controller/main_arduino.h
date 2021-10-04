#ifndef MAIN_ARDUINO_H
#define MAIN_ARDUINO_H

// Debug toggle of verbose interlink comments
#define VERBOSE_INTERLINK 0

// Include headers
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include "board_config.h"
#include "date_time.h"
#include "sensor_log.h"
#include "timer.h"
#include "interconnect.h"
#include "state_flags.h"
#include "settings.h"
#include "bluetooth_interconnect.h"
#include "time_keeper.h"

// Current controller settings and status
Settings settings;

// Sensor log
SensorLog sensorLog;

// Sensor record buffer
constexpr int           sensorRecordMax = 8;
int                     sensorRecordCount = 0;
SensorRecord            sensorRecord[sensorRecordMax];

// Controller timers
// The constructor parameter takes milliseconds, hence the *1000 to convert to seconds
Timer timerReadSensors(5 * 1000);
Timer timerSendToDatabase(10 * 1000);
Timer timerUpdateCurrentTime(1 * 1000);

// A record of the current Epoch time
TimeKeeper timeKeeper;

// A synchronisation-key used when requesting to send data to the database
char sendToDatabaseSyncKey = '0';

// Interconnect buffers
// Used for managing data transfer between different components, such as the Wifi or Bluetooth modules
Interconnect          linkPC(SERIAL_PC);
Interconnect          linkWifi(SERIAL_WIFI);
BluetoothInterconnect linkBT(SERIAL_BLUETOOTH);

// A list of interconnects
// Used when checking who sent a message
enum class LinkId {
	None,
	Wifi,
	PC,
	BT
};
LinkId linkLastRequestToWifi = LinkId::PC;

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
void setup() {

	// Setup serial port for PC connection
	Serial.begin(SERIAL_BITRATE);
	while (!Serial) {}
	Serial.println();

	// Setup serial port for ESP8266 connection
	SERIAL_WIFI.begin(SERIAL_BITRATE); // For ESP8266 connection
	while (!SERIAL_WIFI) {}

	// Setup serial port for Bluetooth module connection
	SERIAL_BLUETOOTH.begin(9600); // For Bluetooth module
	while (!SERIAL_BLUETOOTH) {}

	// Initialise settings data
	Serial.print(F("Loading settings..."));
	settings.init();
	Serial.println(F("Done"));

	// Setup SD card functionality
	Serial.print(F("Initialising SD card..."));
	if (!SD.begin(SD_PIN_CS)) {
		Serial.println(F("Failed"));
		Serial.println(F("Is a card inserted? Is the wiring and pin select correct?"));
		while (true) {}
	} else {
		Serial.println(F("Done"));
	}

	// Setup the sensor log
	Serial.print(F("Initialising sensor log..."));
	if (!sensorLog.begin()) {
		Serial.println(F("Failed"));
		while (true) {}
	} else {
		Serial.println(F("Done"));
	}

	// Output a list of files for debugging
	//Serial.println(F("List of files on the SD card:"));
	//SensorLog::listAllFiles();
	//Serial.println();

	// Final message
	Serial.println(F("Arduino setup finished."));
	Serial.println();
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
		if (timeKeeper.isValid()) {
			sensorLog.record(timeKeeper);
		}
	}

	// Check if it is time to send data to the database
	// Also check if there is actually any data to send
	if (timerSendToDatabase.triggered(currentTime)) {
		if (sensorLog.isUploadRequired() || !settings.isServerInformed()) {
			if (settings.autoUpload()) {
				linkWifi.send(Interconnect::SendToDatabaseRequest, String(sendToDatabaseSyncKey));
				#if VERBOSE_INTERLINK
				linkPC.send(Interconnect::SendToDatabaseRequest, String(sendToDatabaseSyncKey));
				#endif
			}
		}
	}

	// Check if it is time to update the reference for the current time
	if (timerUpdateCurrentTime.triggered(currentTime)) {
		linkWifi.send(Interconnect::GetTime);
		#if VERBOSE_INTERLINK
		linkPC.send(Interconnect::GetTime);
		#endif
	}

	// Update interconnects
	linkWifi.update();
	linkPC.update();
	linkBT.update();

	// Try to receive any pending messages
	LinkId sender = LinkId::None;
	Interconnect::Type header = Interconnect::None;
	ByteQueue payload;
	if (linkWifi.receive(header, payload)) sender = LinkId::Wifi;
	else if (linkPC.receive(header, payload)) sender = LinkId::PC;
	else if (linkBT.receive(header, payload)) sender = LinkId::BT;

	// Process any received message
	if (sender != LinkId::None) {
		Serial.print("Header:");
		Serial.println((char)header);
		switch (header) {



			case Interconnect::ListFiles:
				sensorLog.listAllFiles();
				break;
			case Interconnect::ReadFile:
				if (payload.available()) {
					String s(payload.convertToString());
					Serial.print(F("Read file: "));
					Serial.print(s);
					Serial.println(F("..."));
					bool success = false;
					File f = SD.open(s, FILE_READ);
					if (f) {
						while (f.available()) Serial.write(f.read());
						f.close();
					}
					Serial.println();
					Serial.println(F(" --- End of file ---"));
				} else {
					Serial.print(F("Usage: "));
					Serial.print((char)header);
					Serial.println(F("[filename]"));
				}
				break;
			case Interconnect::DeleteFile:
				if (payload.available()) {
					String s(payload.convertToString());
					Serial.print(F("Removing file: "));
					Serial.print(s);
					Serial.print(F("..."));
					bool success = false;
					File f = SD.open(s);
					if (f) {
						if (f.isDirectory()) {
							f.close();
							success = SD.rmdir(s);
						} else {
							f.close();
							success = SD.remove(s);
						}
					}
					Serial.println(success ? F("Done") : F("Failed"));
				} else {
					Serial.print(F("Usage: "));
					Serial.print((char)header);
					Serial.println(F("[filename]"));
				}
				break;
			case Interconnect::GetRecord:
				if (payload.available()) {
					DateTime dt(payload.convertToString());
					SensorRecord r[4];
					unsigned found = sensorLog.fetch(dt, r, 4);
					Serial.print(F("Found records: "));
					Serial.println(found);
					for (unsigned i=0; i<found; i++) {
						Serial.print(" r[");
						Serial.print(i);
						Serial.print("]: ");
						Serial.println(r[i].toCsv());
					}
				} else {
					Serial.print(F("Usage: "));
					Serial.print((char)header);
					Serial.println(F("[dateTime]"));
				}
				break;




			case Interconnect::SendToDatabaseAllow:
				if (sender == LinkId::Wifi) {

					// Check the synchronisation key
					// If not as expected then this might be an old acknowledgement
					if (sendToDatabaseSyncKey == payload.read()) {

						// Increment synchronisation key for next time
						sendToDatabaseSyncKey = '0' + ((sendToDatabaseSyncKey + 1 - '0') % ('Z' - '0'));

						// Make sure there is actually data to send
						if (sensorLog.isUploadRequired() || !settings.isServerInformed()) {

							// Generate JSON payload
							// This will contain the controller status and/or a list of sensor records
							payload.clear();

							// Add the system GUID
							payload.print(F("{\"Guid\":\""));
							payload.print(settings.systemGuid());
							payload.print('\"');

							// Add the system status, if required
							if (!settings.isServerInformed()) {
								payload.print(F(",\"Status\":"));
								settings.toJson(payload);
							}

							// Add any records which need to be uploaded
							if (sensorLog.isUploadRequired() && sensorLog.lastUploadedIsSet()) {
								payload.print(F(",\"Records\":"));
								sensorLog.fetchForUpload(payload, 16);
							}

							// Finish up
							payload.print('}');

							// Output a copy of the data to the console, if required
							#if VERBOSE_INTERLINK
							{ByteQueue temp(payload.duplicate()); linkPC.send(header, temp);}
							#endif

							// Send data to ESP8266, so that it can forward it to the database
							if (linkWifi.send(Interconnect::SendToDatabase, payload)) {
								settings.setServerInformed();
							}
						}
					}
				}
				break;

			case Interconnect::SendToDatabaseSuccess:
				if (sender == LinkId::Wifi) {

					// Output a copy of the data to the console, if required
					#if VERBOSE_INTERLINK
					{ByteQueue temp(payload.duplicate()); linkPC.send(header, temp);}
					#endif

					// Received notification that the last SendToDatabase was successful
					// Any reply may contain new information from the server
					// Loop through the data and try to apply it
					JsonDecoder decoder(payload);
					while (decoder.fetch()) {
						settings.updateWithKeyValue(decoder.name(), decoder.value()) ||
						sensorLog.updateWithKeyValue(decoder.name(), decoder.value());
					}

					// Check if there is more data to be uploaded
					if (sensorLog.isUploadRequired()) timerSendToDatabase.reset(millis());

					// Output a copy of the data to the console, if required
					#if VERBOSE_INTERLINK
				} else {
					linkPC.send(header, payload);
					#endif
				}
				break;

			case Interconnect::SendToDatabaseFailure:
				if (sender == LinkId::Wifi) {

					// TODO: Improve this. Add better error handling or something
					String msg(F("SendFailure: "));
					msg += payload.convertToString();
					linkPC.send(Interconnect::GeneralNotification, msg);

					// Output a copy of the data to the console, if required
					#if VERBOSE_INTERLINK
					linkPC.send(header, msg);
				} else {
					linkPC.send(header, payload);
					#endif
				}
				break;

			case Interconnect::GeneralNotification: {

				// Received a status message from the ESP8266 which should be passed to the serial port for debugging
				String msg(F("ESP8266: "));
				msg += payload.convertToString();
				linkPC.send(header, msg);
				break;
			}

			case Interconnect::EchoArduino: {

				// Received an echo request from the serial port (used for debugging)
				// Return the same message
				String msg(F("Arduino: Echo: "));
				msg += payload.convertToString();
				switch (sender) {
					case LinkId::PC: linkPC.send(header, msg); break;
					case LinkId::BT: linkBT.send(header, msg); break;
					#if VERBOSE_INTERLINK
					default: linkPC.send(header, msg); break;
					#endif
				}
				break;
			}

			case Interconnect::SetSettings: {

				// Received new settings
				bool result = settings.fromJson(payload);
				payload.clear();
				payload.print(result ? F("Ok") : F("Error"));

				// Send reply
				switch (sender) {
					case LinkId::PC: linkPC.send(header, payload); break;
					case LinkId::BT: linkBT.send(header, payload); break;
					#if VERBOSE_INTERLINK
					default: linkPC.send(header, payload); break;
					#endif
				}
				break;
			}

			case Interconnect::GetSettings:

				// Settings data was requested
				payload.clear();
				settings.toJson(payload);
				switch (sender) {
					case LinkId::PC: linkPC.send(header, payload); break;
					case LinkId::BT: linkBT.send(header, payload); break;
					#if VERBOSE_INTERLINK
					default: linkPC.send(header, payload); break;
					#endif
				}
				break;

			case Interconnect::SetGuid:

				// Save new GUID
				// Send reply
				settings.systemGuid(payload.convertToString());
				settings.save();
				payload.clear();
				payload.print(F("Ok"));
				switch (sender) {
					case LinkId::PC: linkPC.send(header, payload); break;
					case LinkId::BT: linkBT.send(header, payload); break;
					#if VERBOSE_INTERLINK
					default: linkPC.send(header, payload); break;
					#endif
				}
				break;

			case Interconnect::GetGuid:

				// GUID was requested
				payload.clear();
				payload.print(F("GUID:"));
				payload.print(settings.systemGuid());
				switch (sender) {
					case LinkId::PC: linkPC.send(header, payload); break;
					case LinkId::BT: linkBT.send(header, payload); break;
					#if VERBOSE_INTERLINK
					default: linkPC.send(header, payload); break;
					#endif
				}
				break;

			case Interconnect::SetAutoUpload: {

				// Save new auto-upload state
				// Send reply
				char c = (payload.available() > 0) ? payload.read() : 0;
				settings.autoUpload((c != '0') && (c != 'f') && (c != 'F'));
				settings.save();
				payload.clear();
				payload.print(F("Ok"));
				switch (sender) {
					case LinkId::PC: linkPC.send(header, payload); break;
					case LinkId::BT: linkBT.send(header, payload); break;
					#if VERBOSE_INTERLINK
					default: linkPC.send(header, payload); break;
					#endif
				}
				break;
			}

			case Interconnect::GetAutoUnload:

				// Current auto-upload state was requested
				payload.clear();
				payload.print(F("AutoUpload:"));
				payload.print(settings.autoUpload());
				switch (sender) {
					case LinkId::PC: linkPC.send(header, payload); break;
					case LinkId::BT: linkBT.send(header, payload); break;
					#if VERBOSE_INTERLINK
					default: linkPC.send(header, payload); break;
					#endif
				}
				break;

			case Interconnect::SetTime:

				// Output a copy of the data to the console, if required
				#if VERBOSE_INTERLINK
				{ByteQueue temp(payload.duplicate()); linkPC.send(header, temp);}
				#endif

				// Update the reference for calculating the current time
				timeKeeper.current(payload.convertToString());
				timerUpdateCurrentTime.setInterval(60 * 1000);
				switch (sender) {
					case LinkId::PC: linkPC.send(header, String(F("Ok"))); break;
					case LinkId::BT: linkBT.send(header, String(F("Ok"))); break;
				}
				break;

			case Interconnect::GetTime:

				// Current time was requested
				if (timeKeeper.isValid()) {
					switch (sender) {
						case LinkId::PC: linkPC.send(header, timeKeeper.current().toString()); break;
						case LinkId::BT: linkBT.send(header, timeKeeper.current().toString()); break;
						#if VERBOSE_INTERLINK
						default: linkPC.send(header, timeKeeper.current().toString()); break;
						#endif
					}
				}
				break;

			case Interconnect::EchoESP8266:
			case Interconnect::DebugSendToServerKeepHeaders:
			case Interconnect::DebugSendToServer:
			case Interconnect::SetServerAddress:
			case Interconnect::GetServerAddress:
			case Interconnect::GetWifiInfo:

				// Received a command which needs to be forwarded to the ESP8266
				if (sender != LinkId::Wifi) {
					if (linkWifi.send(header, payload)) {
						linkLastRequestToWifi = sender;
					} else {
						String temp(F("Arduino: Interconnect buffer is full"));
						switch (sender) {
							case LinkId::PC: linkPC.send(Interconnect::GeneralNotification, temp); break;
							case LinkId::BT: linkBT.send(Interconnect::GeneralNotification, temp); break;
							#if VERBOSE_INTERLINK
							default: linkPC.send(Interconnect::GeneralNotification, temp); break;
							#endif
						}
					}
				}
				break;

			default:
				break;
		}
	}
}

#endif
