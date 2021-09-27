#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

// Compile target check
#if !defined(ESP8266)
#error "This file should ONLY be included with ESP8266 code"
#endif

// Include headers
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "server_link.h"

// A class used to help with wifi setup tasks
class WifiHelper {
public:

	// Maximum length of the wifi SSID name
	static constexpr int MaxSsidNameLength = 32;

	// Maximum length of the wifi password
	static constexpr int MaxPasswordLength = 32;

	// The EEPROM address used for storing wifi connection info
	static constexpr int EepromAddress = ServerLink::EepromAddress + sizeof(ServerLink);

	// The size required for this class within the EEPROM
	static constexpr int EepromSize = MaxSsidNameLength + MaxPasswordLength;

	// Get the current status of the wifi connection
	// Returns a JSON string
	static String getInfoAsJson() {
		String out;
		out.reserve(32);

		out = F("{\"status\":");
		int status = WiFi.status();
		switch (status) {
			case WL_CONNECTED:       out += F("\"Connected\""); break;
			case WL_NO_SHIELD:       out += F("\"No WiFi shield is present\""); break;
			case WL_IDLE_STATUS:     out += F("\"Trying to connect\""); break;
			case WL_NO_SSID_AVAIL:   out += F("\"No SSID are available\""); break;
			case WL_SCAN_COMPLETED:  out += F("\"Completed scan networks\""); break;
			case WL_CONNECT_FAILED:  out += F("\"Failed to connect\""); break;
			case WL_CONNECTION_LOST: out += F("\"Connection lost\""); break;
			case WL_DISCONNECTED:    out += F("\"Disconnected\""); break;
			default: out += status; break;
		}

		out += F(",\"SSID\":\"");
		out += WiFi.SSID();
		out += '\"';

		if (status == WL_CONNECTED) {

			out += F(",\"IP\":\"");
			out += WiFi.localIP().toString();
			out += '\"';

			out += F(",\"RSSI:");
			out += WiFi.RSSI();

		}

		out += '}';
		return out;
	}

	// Change the wifi connection info
	// The nameAndPassword parameter contains the wifi SSID name, then a plus '+' symbol, then the wifi password
	// Returns true on success, or false if there was a problem with the nameAndPassword parameter
	static bool changeConnectionInfo(const String& nameAndPassword) {
		if (nameAndPassword.length() > 1) {
			int i = nameAndPassword.indexOf('+');
			if (i == -1) {

				// Did not find separator
				// Assume there is no password protection
				return changeConnectionInfo(nameAndPassword.c_str(), nullptr);

			} else if (i > 0) { // Should be greater than zero because there must be a name

				// Found separator
				// Separate the name and password out
				String name = nameAndPassword.substring(0, i);
				String password = nameAndPassword.substring(i + 1);
				return changeConnectionInfo(name.c_str(), password.c_str());
			}
		}
		return false;
	}

	// Change the wifi connection info
	// Returns true on success, or false if something when wrong
	static bool changeConnectionInfo(const char* ssidName, const char* password = nullptr) {

		// Make sure an SSID name was provided
		if (!ssidName || (ssidName[0] == 0)) return false;

		// Determine length of name and password
		int ssidNameLength = strlen(ssidName);
		int passwordLength = password ? strlen(password) : 0;

		// Check length of name and password against the maximum allowed
		if (ssidNameLength > MaxSsidNameLength) return false;
		if (passwordLength > MaxPasswordLength) return false;

		// Copy the connection into to the EEPROM (non-volatile memory)
		for (int i=0; i<MaxSsidNameLength; i++) {
			EEPROM.put(EepromAddress + i, (i < ssidNameLength) ? ssidName[i] : 0);
		}
		for (int i=0; i<MaxPasswordLength; i++) {
			EEPROM.put(EepromAddress + MaxSsidNameLength + i, (i < passwordLength) ? password[i] : 0);
		}
		EEPROM.commit();

		// Disconnect from current wifi network, if any
		WiFi.disconnect();

		// Start the wifi using the new SSID and password
		WiFi.begin(ssidName, password);

		// Return success
		return true;
	}

	// Setup and start the wifi, using stored connection info
	static void begin() {

		// Define buffers to hold the wifi SSID name and password
		// Make the buffers one character longer than maximum
		// Set the last character to zero to act as a null-terminated in case the string is at maximum length
		char ssidName[MaxSsidNameLength + 1];
		char password[MaxSsidNameLength + 1];
		ssidName[MaxSsidNameLength] = 0;
		password[MaxPasswordLength] = 0;

		// Copy the address data from the EEPROM (non-volatile memory)
		for (int i=0; i<MaxSsidNameLength; i++) {
			ssidName[i] = EEPROM.read(EepromAddress + i);
		}
		for (int i=0; i<MaxPasswordLength; i++) {
			password[i] = EEPROM.read(EepromAddress + MaxSsidNameLength + i);
		}

		// Explicitly set the ESP8266 to be a wifi-client, otherwise, it by default
		// would try to act as both a client and an access-point and could cause
		// network-issues with your other wifi-devices on your wifi-network.
		WiFi.mode(WIFI_STA);

		// Start the wifi using the stored SSID and password
		if (ssidName[0] == 0) {
			WiFi.begin();
		} else if (password[0] == 0) {
			WiFi.begin(ssidName);
		} else {
			WiFi.begin(ssidName, password);
		}
	}
};

#endif
