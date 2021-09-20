#ifndef MAIN_ESP8266_H
#define MAIN_ESP8266_H

// Include headers
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "interconnect.h"
#include "state_flags.h"
#include "server_link.h"
#include "wifi_helper.h"

// State flags transmitting
#define FLAG_PENDING_DATA_FOR_DATABASE (StateFlags::Type(1 << 0))

// Declare global values
StateFlags   stateFlags();
Interconnect linkArduino(Serial, INTERCONNECT_BUFFER_ESP8266_TO_ARDUINO, INTERCONNECT_BUFFER_ARDUINO_TO_ESP8266);
char         requestSendDataForDatabaseKey = 0;
String       dataForDatabase;
ServerLink   linkServer;

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
// Note: Anything sent via Serial within setup() does NOT reach the Arduino!!!
void setup() {

	// Open a serial connection to the Arduino chip
	Serial.begin(SERIAL_BITRATE_ARDUINO_ESP8266);

	// Setup the EEPROM data
	// This will use a buffer of 512 bytes
	EEPROM.begin(512);

	// Setup and start the wifi, using stored connection info
	WifiHelper::begin();

	// Initialise server data
	linkServer.init();
}

// Runs repeatedly once the Arduino has finished initialisation
// This function will only run after setup() has completed
void loop() {

	// Update interconnect
	linkArduino.update();

	// Check for received messages
	Interconnect::Type header = Interconnect::None;
	String payload;
	if (linkArduino.receive(header, payload)) {

		// Process received message
		switch (header) {

			case Interconnect::DataForDatabase: {

				// Received data which needs to be sent to the database
				// Try to send the data
				// Record the data if not sent
				String reply;
				if (!linkServer.sendJson(payload, reply)) dataForDatabase = payload;
				break;
			}

			case Interconnect::RequestSendDataForDatabase:

				// Received a request to be allowed to send a large message
				// A key will have been sent with this message
				// Record the key for later
				requestSendDataForDatabaseKey = payload[0];
				break;

			case Interconnect::DebugSendToServerKeepHeaders:
			case Interconnect::DebugSendToServer: {

				// Received data which needs to be sent to the database
				// Try to send the data
				// Record the data if not sent
				String reply;
				if (linkServer.sendJson(payload, reply, header == Interconnect::DebugSendToServerKeepHeaders)) {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Sent to server. Reply: ")) + reply);
				} else {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Error sending to server. Reply: ")) + reply);
				}
				break;
			}

			case Interconnect::SetWifiInfo:
				if (WifiHelper::changeConnectionInfo(payload)) {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Changed Wifi: ")));
				} else {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Error changing wifi")));
				}
				break;

			case Interconnect::GetWifiInfo:
				linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Wifi info: ")) + WifiHelper::getInfoAsJson());
				break;

			case Interconnect::SetServerAddress:
				if (linkServer.changeAddress(payload)) {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Server address changed to: ")) + payload);
				} else {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Error changing server address to: ")) + payload);
				}
				break;

			case Interconnect::GetServerAddress:
				linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Server address: ")) + linkServer.getAddress());
				break;

			case Interconnect::EchoESP8266:
				linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Echo: ")) + payload);
				break;

			default:
				break;
		}
	}

	// Check if there is anything which needs to be done
	if (dataForDatabase.length() > 0) {

		// There is data waiting to be sent to the database
		// Try to send it and clear the buffer value if successful
		String reply;
		if (linkServer.sendJson(dataForDatabase, reply)) {
			dataForDatabase = String();
		}

	} else if (requestSendDataForDatabaseKey && linkArduino.emptySendBuffer()) {

		// There is a request-to-send-data-key
		// If here then everything else must have been finished, so send a reply and clear the key
		// Send the synchronisation key back with the allow message
		linkArduino.send(Interconnect::AllowSendDataForDatabase, String(requestSendDataForDatabaseKey));
		requestSendDataForDatabaseKey = 0;
	}
}

#endif
