#ifndef MAIN_ESP8266_H
#define MAIN_ESP8266_H

// Include headers
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "interconnect.h"
#include "state_flags.h"
#include "server_link.h"
#include "wifi_helper.h"

// State flags transmitting
#define FLAG_PENDING_DATA_FOR_DATABASE (StateFlags::Type(1 << 0))

// Declare global values
Interconnect linkArduino(Serial, INTERCONNECT_BUFFER_ESP8266_TO_ARDUINO, INTERCONNECT_BUFFER_ARDUINO_TO_ESP8266);
ServerLink   linkServer;
bool         expectSendToDatabase = false;
WiFiUDP      ntpUDP;
NTPClient    ntpClient(ntpUDP, "au.pool.ntp.org", 0, 60 * 60 * 24);
bool         ntpClientValid = false;

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
// Note: Anything sent via Serial within setup() does NOT reach the Arduino!!!
void setup() {

	// Open a serial connection to the Arduino chip
	Serial.begin(SERIAL_BITRATE);

	// Setup the EEPROM data
	// This will use a buffer of 512 bytes
	EEPROM.begin(512);

	// Setup and start the wifi, using stored connection info
	WifiHelper::begin();

	// Initialise server data
	linkServer.init();

	// Initialise the NTP Client
	ntpClient.begin();
}

// Runs repeatedly once the Arduino has finished initialisation
// This function will only run after setup() has completed
void loop() {

	// Update current time, but only if idle
	if (!expectSendToDatabase) {
		ntpClientValid |= ntpClient.update();
	}

	// Update interconnect
	linkArduino.update();

	// Check for received messages
	Interconnect::Type header = Interconnect::None;
	String payload;
	if (linkArduino.receive(header, payload)) {

		// Process received message
		switch (header) {

			case Interconnect::SendToDatabaseRequest:

				// Received a request to be allowed to send data to the database
				// A key will have been sent with this message
				// If able to send then reply with the key
				if (WiFi.status() == WL_CONNECTED) {
					linkArduino.send(Interconnect::SendToDatabaseAllow, payload);
					expectSendToDatabase = true;
				} else {
					payload = F("Wifi not connected");
					linkArduino.send(Interconnect::SendToDatabaseDisallow, payload);
				}
				break;

			case Interconnect::SendToDatabase: {

				// Received data which needs to be sent to the database
				// Try to send the data
				// Record the data if not sent
				String reply;
				if (linkServer.sendJson(payload, reply)) {
					linkArduino.sendForce(Interconnect::SendToDatabaseSuccess, reply);
				} else {
					linkArduino.sendForce(Interconnect::SendToDatabaseFailure, reply);
				}
				expectSendToDatabase = false;
				break;
			}

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

			case Interconnect::GetTime:
				if (ntpClientValid) linkArduino.send(Interconnect::SetTime, String(ntpClient.getEpochTime()));
				break;

			case Interconnect::EchoESP8266:
				linkArduino.sendForce(Interconnect::GeneralNotification, String(F("Echo: ")) + payload);
				break;

			default:
				break;
		}
	}
}

#endif
