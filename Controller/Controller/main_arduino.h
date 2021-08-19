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
Timer        timerReadSensors(5 * 1000);
Timer        timerCheckForCommands(60 * 1000);
Timer        timerInterconnectPing(2500);
Interconnect interconnect;
char         sendLargeMessageSyncKey = '0';

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
void setup() {

	// Setup serial connections
	Serial.begin(SERIAL_BITRATE_ARDUINO_PC); // For PC connection
	Serial3.begin(SERIAL_BITRATE_ARDUINO_ESP8266); // For ESP8266 connection

	/*
	Serial.println("-----Add-----");
	interconnect.send(Interconnect::EchoESP8266, String("T1"));
	Serial.println("-----Add-----");
	interconnect.send(Interconnect::EchoESP8266, String("T2"));
	Serial.println("-----Info-----");
	interconnect.debugDump();
	Serial.println("-----Sub-----");
	for (int i=0; i<4; i++) interconnect.getNextByteToSend();
	Serial.println("-----Add-----");
	interconnect.send(Interconnect::EchoESP8266, String("T3"));
	Serial.println("-----Info-----");
	interconnect.debugDump();
*/

	// Debug message
	//Serial.println(F("Arduino starting..."));
}

// Runs repeatedly once the Arduino has finished initialisation
// This function will only run after setup() has completed
void loop() {

	if (Serial.available() > 0) {
		int header = Serial.read();
		String data = Serial.readStringUntil('\r');
		data.trim();
		interconnect.send(header, data);
	}//*/

	/*
	sensorRecord.readAll();
	Serial.println(sensorRecord.toJSON());
	delay(800);
	return; //*/

	/*
	//while (true) {
		if (Serial3.available() > 0) Serial.write(Serial3.read());
		if (Serial.available() > 0) Serial3.write(Serial.read());
		//if (Serial.available() > 0) Serial.write(Serial.read());
	//}
	return; //*/

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

	// Receive data from ESP8266
	while (Serial3.available() > 0) {
		if (interconnect.receiveByte(Serial3.read())) {

			// A message is ready
			Interconnect::Type type = interconnect.getReceivedType();
			String data = interconnect.getReceivedMessage();
			switch (type) {

				case Interconnect::GeneralNotification:
					Serial.print(F("From ESP8266: "));
					Serial.println(data);
					break;

				case Interconnect::AllowSendDataForDatabase:

					// Check the synchronisation key
					// If not as expected then this might be an old acknowledgement
					if (sendLargeMessageSyncKey == data[0]) {

						// Increment synchronisation key for next time
						sendLargeMessageSyncKey = '0' + ((sendLargeMessageSyncKey + 1 - '0') % ('Z' - '0'));

						// Allowed to send data for database to the ESP8266
						if (stateFlags.anySet(FLAG_PENDING_SENSOR_RECORD)) {
							if (interconnect.send(Interconnect::DataForDatabase, sensorRecord.toJSON())) {
								stateFlags.reset(FLAG_PENDING_SENSOR_RECORD);
							} else {
								Serial.println(F("Failed to add sensor data to Interconnect"));
							}
						} else if (stateFlags.anySet(FLAG_PENDING_CURRENT_STATUS)) {
							// TODO: Add code to send current controller status
							stateFlags.reset(FLAG_PENDING_CURRENT_STATUS);
						}

						// Now that data has been added to the send queue, reset the large data timer
						timerInterconnectPing.reset(currentTime);
					}
					break;

				default:
					break;
			}
		}
	}

	// Check if any data which needs to be sent to the database, via the ESP8266
	if (stateFlags.anySet(FLAG_PENDING_DATA_FOR_DATABASE)) {
		if (interconnect.emptySendBuffer() && timerInterconnectPing.triggered(currentTime)) {
			interconnect.send(Interconnect::RequestSendDataForDatabase, String(sendLargeMessageSyncKey));
		}
	}

	// Send data to ESP8266, if allowed and if able
	while (interconnect.sendRequired() && (Serial3.availableForWrite() > 0)) {
		Serial3.write(interconnect.getNextByteToSend());
	}
}

#endif
