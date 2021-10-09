#ifndef MAIN_ESP8266_H
#define MAIN_ESP8266_H

// Include headers
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "board_config.h"
#include "interconnect.h"
#include "state_flags.h"
#include "server_link.h"
#include "wifi_helper.h"
#include "byte_queue.h"
#include "timer.h"

// State flags transmitting
#define FLAG_PENDING_DATA_FOR_DATABASE (StateFlags::Type(1 << 0))

// Declare global values
Interconnect linkArduino(Serial);
ServerLink   linkServer;
WiFiUDP      ntpUDP;
NTPClient    ntpClient(ntpUDP, "au.pool.ntp.org", 0, 60 * 60 * 24);
bool         ntpClientValid = false;
bool         expectSendToDatabase = false;
Timer        timerUpdateTimeClient(1 * 1000);
Timer        timerLastSend(5 * 1000);

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

	// Update interconnect
	linkArduino.update();

	// Check for received messages
	Interconnect::Type header = Interconnect::None;
	ByteQueue payload;
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
					payload.clear();
					payload.print(F("Wifi not connected"));
					linkArduino.send(Interconnect::SendToDatabaseDisallow, payload);
				}
				break;

			case Interconnect::SendToDatabase: {

				// Received data which needs to be sent to the database
				// Try to send the data
				// Record the data if not sent
				ByteQueue reply;
				if (linkServer.sendJson(payload.convertToString(), reply)) {
					linkArduino.send(Interconnect::SendToDatabaseSuccess, reply);
				} else {
					linkArduino.send(Interconnect::SendToDatabaseFailure, reply);
				}
				expectSendToDatabase = false;
				break;
			}

			case Interconnect::DebugSendToServerKeepHeaders:
			case Interconnect::DebugSendToServer: {

				// Received data which needs to be sent to the database
				// Try to send the data
				// Record the data if not sent
				ByteQueue reply;
				if (linkServer.sendJson(payload.convertToString(), reply, header == Interconnect::DebugSendToServerKeepHeaders)) {
					linkArduino.send(Interconnect::GeneralNotification, String(F("Sent to server. Reply: ")) + reply.convertToString());
				} else {
					linkArduino.send(Interconnect::GeneralNotification, String(F("Error sending to server. Reply: ")) + reply.convertToString());
				}
				break;
			}

			case Interconnect::SetWifiInfo:
				if (WifiHelper::changeConnectionInfo(payload.convertToString())) {
					linkArduino.send(Interconnect::GeneralNotification, String(F("Changed Wifi: ")));
				} else {
					linkArduino.send(Interconnect::GeneralNotification, String(F("Error changing wifi")));
				}
				break;

			case Interconnect::GetWifiInfo:
				linkArduino.send(Interconnect::GeneralNotification, String(F("Wifi info: ")) + WifiHelper::getInfoAsJson());
				break;

			case Interconnect::SetServerAddress:
				if (linkServer.changeAddress(payload.convertToString())) {
					linkArduino.send(Interconnect::GeneralNotification, String(F("Server address changed to: ")) + payload.convertToString());
				} else {
					linkArduino.send(Interconnect::GeneralNotification, String(F("Error changing server address to: ")) + payload.convertToString());
				}
				break;

			case Interconnect::GetServerAddress:
				linkArduino.send(Interconnect::GeneralNotification, String(F("Server address: ")) + linkServer.getAddress());
				break;

			case Interconnect::GetTime:
				if (ntpClientValid) linkArduino.send(Interconnect::SetTime, String(ntpClient.getEpochTime()));
				break;

			case Interconnect::EchoESP8266: {
				String msg(F("Echo: "));
				msg += payload.convertToString();
				msg += " : expectSendToDatabase=";
				msg += expectSendToDatabase;
				msg += ", ntpClientValid=";
				msg += ntpClientValid;
				linkArduino.send(Interconnect::GeneralNotification, msg);
				break;
			}

			default:
				break;
		}
	} else {

		// If here then there are no commands which need processing
		// Update current time, but only if idle
		if (!expectSendToDatabase && timerUpdateTimeClient.triggered(millis())) {
			ntpClientValid |= ntpClient.update();
		}

	}
}

#endif
