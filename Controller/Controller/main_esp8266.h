#ifndef MAIN_ESP8266_H
#define MAIN_ESP8266_H

// Include headers
#include <ESP8266WiFi.h>
#include "interconnect.h"
#include "state_flags.h"
#include "server_link.h"

// This file includes the connection info for the Arduino, but it is not included in the GitHub source
// This is to allow different developers with different local wifi networks to setup the project accordingly
// To create this file, make a copy of "connection_info_template.h.txt" and rename it to "connection_info.h"
// Within the copy, fill out the information as desired
#include "connection_info.h"

// State flags transmitting
#define FLAG_PENDING_DATA_FOR_DATABASE (StateFlags::Type(1 << 0))

// Declare global values
StateFlags   stateFlags();
Interconnect linkArduino(Serial, INTERCONNECT_BUFFER_ESP8266_TO_ARDUINO, INTERCONNECT_BUFFER_ARDUINO_TO_ESP8266);
String       wifiSSID        = WIFI_SSID;
String       wifiPassword    = WIFI_PASSWORD;
//String       databaseAddress = DATABASE_ADDRESS;
//uint16_t     databasePort    = DATABASE_PORT;
char         requestSendDataForDatabaseKey = 0;
String       dataForDatabase;
ServerLink   linkServer;

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
// Note: Anything sent via Serial within setup() does NOT reach the Arduino!!!
void setup() {

	// Open a serial connection to the Arduino chip
	Serial.begin(SERIAL_BITRATE_ARDUINO_ESP8266);

	// Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default
	// would try to act as both a client and an access-point and could cause
	// network-issues with your other WiFi-devices on your WiFi-network.
	WiFi.mode(WIFI_STA);

	// Start the wifi using the hard-coded SSID and password
	// TODO: This will need to be changed to instead use non-hard-coded values,
	// probably using PROGMEM instead: https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
	WiFi.begin(wifiSSID, wifiPassword);

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

			case Interconnect::DebugSendToServer: {

				// Received data which needs to be sent to the database
				// Try to send the data
				// Record the data if not sent
				String reply;
				if (linkServer.sendJson(payload, reply, true)) {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Sent to server. Reply: ")) + reply);
				} else {
					linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Error sending to server. Reply: ")) + reply);
				}
				break;
			}

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
