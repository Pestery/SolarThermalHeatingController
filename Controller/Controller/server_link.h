#ifndef SERVER_LINK_H
#define SERVER_LINK_H

/***** Example POST request *******
POST /echo/post/json HTTP/1.1
Host: website.com
Accept: application/json
Content-Type: application/json
Content-Length: 81

{
	"id":9,
	"value":50.0,
	"readDate":"2005-09-01T13:00:20"
}
***********************/

// Compile target check
#if !defined(ESP8266)
#error "This file should ONLY be included with ESP8266 code"
#endif

// Include headers
#include <WiFiClientSecure.h>
#include <EEPROM.h>
#include "server_address.h"
#include "timer.h"
#include "byte_queue.h"

// A class used to manage communication with the server/database
class ServerLink {
public:

	// The EEPROM address used for storing server info
	static constexpr int EepromAddress = 0;

	// Send some data to the database
	// A POST request is used as the container, and the data must be in JSON format
	// Any reply from the server is stored in the 'reply' parameter
	// In the case of an error, the error message maybe stored in the 'reply' parameter
	// Returns true on success, or false on failure
	bool sendJson(const String& payload, ByteQueue& reply, bool keepHeaders = false) {

		// Make sure the wifi is currently connected
		if (WiFi.status() != WL_CONNECTED) {
			reply.clear();
			reply.print(F("Wifi not connected"));
			return false;
		}

		// Check if using a secure connection
		if (m_address.secure()) {

			// Define the WiFiClientSecure class, which is used to create a secure TCP connection
			WiFiClientSecure client;

			// Set the connection as insecure
			// This will allow an SSL connection, but will not validate the certificate of the server
			// TODO: This is VERY insecure and should be changed in future!!!
			client.setInsecure(); // <---- VERY INSECURE!!!

			// Try to connect to the server
			if (client.connect(m_address.host(), m_address.port())) {

				// Send message to the server, receive the reply, and close the connection
				bool result = internalSendJson(client, payload, reply, keepHeaders);
				client.stop();
				return result;
			}

		} else {

			// Define the WiFiClient class, which is used to create an insecure TCP connection
			WiFiClient client;

			// Try to connect to the server
			if (client.connect(m_address.host(), m_address.port())) {

				// Send message to the server, receive the reply, and close the connection
				bool result = internalSendJson(client, payload, reply, keepHeaders);
				client.stop();
				return result;
			}
		}

		// If here then failed to connect to server
		// Return failed to send data
		reply.clear();
		reply.print(F("Failed to connect to server"));
		return false;
	}

	// Change the server address and port
	// Returns true on success, or false if there was something wrong with the new address
	bool changeAddress(const String& newAddress) {

		// Verify that the new address if valid
		// If valid then save data
		if (m_address.set(newAddress)) {

			// Copy the address data to the EEPROM (non-volatile memory)
			EEPROM.put(EepromAddress, *this);
			EEPROM.commit();

			// Result result
			return true;
		} else {
			return false;
		}
	}

	// Get the current server address and port
	String getAddress() const {
		return m_address.get();
	}

	// Initialise the server data
	void init() {

		// Copy the address data from the EEPROM (non-volatile memory)
		EEPROM.get(EepromAddress, *this);
	}

	// Default constructor
	ServerLink() :
		m_address() {
	}

private:

	// Send some data to the database
	// A POST request is used as the container, and the data must be in JSON format
	// This method is used internally by sendJson() to manage sending and receiving data from the server
	// Returns true on success, or false on failure
	bool internalSendJson(WiFiClient& client, const String& payload, ByteQueue& reply, bool keepHeaders) {
		bool result = false;

		// Send POST request
		sendPostRequestJson(client, payload);

		// Wait for and process reply
		int statusCode = 0;
		if (processReplyJson(client, statusCode, reply, keepHeaders)) result = true;

		// Check the status code
		// Return result
		switch (statusCode) {
			case 200:
				return result;
			case 204: // TODO: This could probably be handled better
				result = F("{}");
				return result;
		}
		return false;
	}

	// Send some data to the database
	// A POST request is used as the container, and the data must be in JSON format
	// This method is used internally by sendJson() to manage sending data to the server
	void sendPostRequestJson(WiFiClient& client, const String& payload) {
		String header;

		// POST /echo/post/json HTTP/1.1
		header = F("POST ");
		header += m_address.path();
		header += F(" HTTP/1.1\r\n");

		// Host: website.com
		header += F("Host: ");
		header += m_address.host();
		header += ':';
		header += m_address.port();
		header += F("\r\n");

		// Accept: application/json
		// Content-Type: application/json
		header += F("Accept: application/json\r\n");
		header += F("Content-Type: application/json\r\n");

		// Content-Length: (some length)
		header += F("Content-Length: ");
		header += payload.length();
		header += F("\r\n");

		// Empty line which marks the end of the header
		header += F("\r\n");

		// Send header and payload
		client.print(header);
		client.print(payload);
		client.print(F("\r\n"));
	}

	// Process the reply after sending JSON data via sendJson()
	// A POST request is used as the container, and the data must be in JSON format
	// Returns true on success, or false on failure
	bool processReplyJson(WiFiClient& client, int& statusCode, ByteQueue& reply, bool keepHeaders) {
		unsigned contentLength = 0;
		bool chunkedReply = false;
		String temp;
		temp.reserve(100);
		reply.clear();

		// Get the status line of the response
		// This should be the first line returned, but try a few times if the line is blank
		for (int i=0; i<4; i++) {
			temp = getNextLine(client);
			if (keepHeaders) reply.println(temp);
			if (temp.length() > 0) break;
		}

		// Get status code, from the status line
		statusCode = getStatusCode(temp.c_str());
		if (!statusCode) {
			if (!keepHeaders) {
				reply.print(F("Failed to get status-code from reply from:"));
				reply.print(temp);
			}
			return false;
		}

		// Loop through headers
		// This section will end when an empty line is found
		while (true) {

			// Get next line from header section
			temp = getNextLine(client);
			if (keepHeaders) reply.println(temp);

			// Check for empty line, which marks the end of the header section
			if (temp.length() == 0) break;

			// Get location of colon separator within header
			const char* i = strchr(temp.c_str(), ':');
			if (i) {

				// Get header component
				String header = Misc::makeString(temp.c_str(), i - temp.c_str());
				header.trim();
				header.toLowerCase();

				// Check for useful headers
				if (header == F("content-length")) {
					temp = temp.substring((i - temp.c_str()) + 1);
					temp.trim();
					contentLength = temp.toInt();
				} else if (header == F("transfer-encoding")) {
					temp = temp.substring((i - temp.c_str()) + 1);
					temp.trim();
					temp.toLowerCase();
					if (temp == F("chunked")) {
						chunkedReply = true;
					} else {
						if (!keepHeaders) reply.print(F("Server sent unrecognised format in reply"));
						return false;
					}
				}
			}
		}

		// If here then found end of header section
		// Check if there is a payload
		if (contentLength > 0) {

			// Check if keeping headers
			if (keepHeaders) {
				reply.println(getNextLength(client, contentLength));
			} else {
				reply.print(getNextLength(client, contentLength));
			}
		} else if (chunkedReply) {

			// Check if keeping headers
			if (keepHeaders) {
				reply.println(getNextChunked(client));
			} else {
				reply.print(getNextChunked(client));
			}
		}
		return true;
	}

	// Get the status code returned by the server
	// The status code is part of the status line, which is the first line received back from the server
	// Returns zero on error, or the response code on success (the response code may signal an error)
	int getStatusCode(const char* source) {

		// Make sure the protocol version is correct (check the line begins with "HTTP/1.1")
		const char* protocol = "HTTP/1.1";
		int length = strlen(protocol);
		if (strncmp(protocol, source, length)) return 0;
		source += length;

		// Skip white spaces
		while (*source && isWhitespace(*source)) source++;

		// Get length of status code number
		length = 0;
		while (source[length] && !isWhitespace(source[length])) length++;

		// Get the status code
		return Misc::makeString(source, length).toInt();
	}

	// Get the next line from a WiFiClient
	// This simply looks for the next end-line character '\n' and returns everything up to that point
	// Any carriage-return characters '\r' are ignored
	String getNextLine(WiFiClient& client) {
		String temp;
		Timer timeOut(15 * 1000, millis());

		// Loop forever
		while (true) {

			// Check if data is available to read
			// Also check if timed-out waiting, or if connection has closed
			while (client.available() == 0) {
				if (timeOut.triggered(millis()) || !client.connected()) {
					return temp;
				}
			}

			// Get next byte
			int b = client.read();
			switch (b) {

			case -1:
				// Ignore failed reads (not error)
				break;

			case '\r':
				// Ignore return characters
				break;

			case '\n':
				// Found end-of-line
				// Return result
				return temp;

			default:
				// Add value to string
				temp += (char)b;
				break;
			}
		}
		return temp;
	}

	// Get the next arbitrary amount of data from a WiFiClient
	// This will simply copy the next 'length' worth of data from the stream and return it as a string
	String getNextLength(WiFiClient& client, unsigned length) {
		String result;
		result.reserve(length);
		Timer timeOut(60 * 1000, millis());

		// Loop forever
		while (true) {

			// Check if data is available to read
			// Also check if timed-out waiting, or if connection has closed
			while (client.available() == 0) {
				if (timeOut.triggered(millis()) || !client.connected()) {
					return result;
				}
			}

			// Get next byte
			int b = client.read();
			if (b != -1) {
				length--;
				result += (char)b;
			}
		}
		return result;
	}

	// Get the next arbitrary amount of data from a WiFiClient
	// This will simply copy the next amount of data from the stream and return it as a string
	// The data is expected to be in the format "Transfer-Encoding: chunked"
	// See: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding
	String getNextChunked(WiFiClient& client) {
		String result;
		result.reserve(128);
		Timer timeOut(60 * 1000, millis());
		unsigned length = 0;
		bool processEndOfLine = false;

		// Loop forever
		while (true) {

			// Check if data is available to read
			// Also check if timed-out waiting, or if connection has closed
			while (client.available() == 0) {
				if (timeOut.triggered(millis()) || !client.connected()) {
					return result;
				}
			}

			// Check what to do
			if (processEndOfLine) {

				// Check and clear the bytes used to mark the end of a line
				// This should be "\r\n"
				int b = client.read();
				if (b == '\r') {
					// Do nothing
				} else if (b == '\n') {
					// Found new-line character
					processEndOfLine = false;
				} else if (b == -1) {
					// Soft error. Failed to read next byte. Just ignore the error
				} else {
					// Error. Unexpected byte
					return result;
				}

			} else if (length == 0) {

				// Get the next 'length' line
				// This line will contain the length of the following line of data, in hexadecimal
				// If the stated length is zero then reached end of data
				String lengthString = getNextLine(client);
				lengthString.trim();
				length = Misc::hexToInt(lengthString.c_str());
				if (length == 0) return result;

			} else {

				// Get next byte
				// Also check if end of chunk
				int b = client.read();
				if (b != -1) {
					result += (char)b;
					if (--length == 0) {
						processEndOfLine = true;
					}
				}
			}
		}
		return result;
	}

	// Private data
	ServerAddress m_address;
};

#endif
