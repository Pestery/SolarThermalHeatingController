#ifndef INTERCONNECT_H
#define INTERCONNECT_H

// Include headers
#include "byte_queue.h"

// Bit-rate for serial communication between Arduino, ESP8266 and PC
// Must be kept the same for all serial connections because otherwise buffer overflows may
// occur when forwarding from one serial to another, if one bit-rate is higher than the another
#define SERIAL_BITRATE 9600

// Interconnect buffer sizes for different connections and direction of connections
// This should be set to reflect the expected maximum size of message to be sent on that link
#define INTERCONNECT_BUFFER_ARDUINO_TO_ESP8266   128
#define INTERCONNECT_BUFFER_ARDUINO_TO_BLUETOOTH 64
#define INTERCONNECT_BUFFER_ARDUINO_TO_PC        64
#define INTERCONNECT_BUFFER_ESP8266_TO_ARDUINO   64
#define INTERCONNECT_BUFFER_BLUETOOTH_TO_ARDUINO 32
#define INTERCONNECT_BUFFER_PC_TO_ARDUINO        32

// Used to make communication between the Arduino and ESP8266 a bit easier
class Interconnect {
public:

	// A list of possible command types
	// Notes:
	//  - An upper-case 'O' must not be used for any command letter, as it is used for Bluetooth module messages
	//  - All messages must begin with the command letter, optionally have a payload message, and end with a new line character '\n'
	//  - A carriage return character '\b' is used to ask the other side to pause sending data because this side may not be able to receive it
	//  - Any new line characters '\n' ands carriage return character '\b' must be escaped (slash followed by letter, instead of actual value)
	enum Type : uint8_t {

		// A small request send from the Arduino to the ESP8266
		// Used to request permission to send a SendToDatabase message
		// These messages might saturate the send buffer and must be processed in a timely fashion
		// Payload is a single byte, used to match against SendToDatabaseAllow, to identify duplicate requests
		SendToDatabaseRequest = 'r',

		// A small acknowledgement sent from the ESP8266 to the Arduino
		// Used in response to a SendToDatabaseRequest, to allow the requested SendToDatabase message
		// Payload is a single byte, the same as sent with SendToDatabaseRequest
		SendToDatabaseAllow = 'a',

		// A small reply sent from the ESP8266 to the Arduino
		// Used in response to a SendToDatabaseRequest, to disallow the requested SendToDatabase message
		// This message might be sent if, for example, the wifi is not currently connected
		// No payload
		SendToDatabaseDisallow = 'n',

		// The header for a message which contain data destined for the database
		// Sent from the Arduino to the ESP8266
		// The payload must be a JSON string
		SendToDatabase  = 'd',

		// A header used to indicate that an error occurred while sending data to database
		// Sent by the the ESP8266 to the Arduino, in response to a SendToDatabase message
		// Payload (optional) may contain information about the error
		SendToDatabaseFailure = 'f',

		// A header used to indicate a successful database transaction
		// Sent by the the ESP8266 to the Arduino, in response to a SendToDatabase message
		// Payload (optional) may contain a reply from the server in the form of a JSON string
		SendToDatabaseSuccess = 'k',

		// A notification message which should be forwarded to the user, probably via the serial port
		// Do not use. Intended for system messages back to user
		// Payload is the notification message
		GeneralNotification = ':',

		// Set the server address
		// Payload is the protocol, website and web page
		// The general format should be similar to: http://localhost:5000/api/System
		SetServerAddress = 's',

		// Get the server address
		// No payload
		GetServerAddress = 'S',

		// Set the controller settings/configuration data
		// Payload is a JSON object with the settings to be changed
		// For example: {"auto":true}
		SetSettings = 'c',

		// Get the controller settings/configuration and status data
		// No payload
		GetSettings = 'C',

		// Set the wifi SSID name and password
		// The payload should be either:
		//  - The wifi name
		//  - The wifi name, followed by plus '+' symbol, followed by wifi password
		SetWifiInfo = 'w',

		// Request the wifi status
		// No payload
		GetWifiInfo = 'W',

		// Set the GUID of the system
		// The payload should be the GUID string which the database expects
		// The GUID string should be a hexadecimal string similar to: 12345678-1234-1234-1234-123456789012
		SetGuid = 'g',

		// Request the wifi status
		// No payload
		GetGuid = 'G',

		// Request the current time
		// No payload
		GetTime = 'E',

		// Set the current time
		// Payload is the current Epoch time as a string
		SetTime = 'e',

		// A debug command used to toggle automatic uploading of data to database
		// Payload should be either a '0' or '1', to represent either false or true respectively
		SetAutoUpload = 'u',
		GetAutoUnload = 'U',

		// A debug command used to send data to server
		// The response will be forwarded back through to the serial port
		// Payload is the (probably JSON) message to be sent to the server
		DebugSendToServerKeepHeaders = 'T',
		DebugSendToServer = 't',

		// A debug command used to echo back data to the source
		// Payload is the test message
		EchoArduino = 'x',
		EchoESP8266 = 'y',

		// A header used to mark an error
		Error = '*',

		// Used to indicate no message
		None = 0
	};

	// Update the interconnect
	// This will send and receive any waiting data in the related buffers
	void update() {

		// Receive data
		// Check if any data is available
		while (m_stream.available()) {

			// Try to add the data to the buffer
			uint8_t c = m_stream.read();
			if (!m_recv.write(c)) {

				// Failed to add data
				// Erase what is there and record the error
				m_recv.clear();
				m_recv.write(Interconnect::Error);
				if (c == '\n') m_recv.write(c);
			}

			// Check for control characters
			if (c == '\n') m_receivedMessages++;
			m_receivedAnything = true;
		}

		// Send data
		while (m_send.available() && (m_stream.availableForWrite() > 0)) {
			m_stream.write(m_send.read());
		}
	}

	// Return true if the send buffer is empty
	int emptySendBuffer() const {
		return !m_send.available();
	}

	// Returns the number of received messages within the receive buffer
	int waitingMessages() const {
		return m_receivedMessages;
	}

	// Returns true if anything has been received since the last time this method was called
	bool receivedAnything() {
		if (m_receivedAnything) {
			m_receivedAnything = false;
			return true;
		} else {
			return false;
		}
	}

	// Add data to the send queue
	// Returns true on success, or false if there was not enough space in the buffer
	bool send(Type header) {
		return send(header, nullptr);
	}
	bool send(Type header, const String& payload) {
		return send(header, payload.c_str());
	}
	bool send(Type header, const char* payload) {

		// Set a roll-back point
		ByteQueue::Rollback rollback(m_send);

		// Add message header
		if (!m_send.write(header)) {
			rollback.rollback();
			return false;
		}

		// Add message body, if any
		if (payload) {
			uint8_t c, e;
			while (*payload) {
				c = *(payload++);
				e = getEscapedVersion(c);
				if (e) {
					m_send.write('\\');
					m_send.write(e);
				} else {
					m_send.write(c);
				}
			}
		}

		// Add message tail
		if (!m_send.write('\n')) {
			rollback.rollback();
			return false;
		}
		return true;
	}
	bool send(Type header, ByteQueue& payload) {

		// Set a roll-back point
		ByteQueue::Rollback rollback(m_send);

		// Add message header
		if (!m_send.write(header)) {
			rollback.rollback();
			return false;
		}

		// Add message body, if any
		if (payload.available()) {
			uint8_t c, e;
			while (payload.available()) {
				c = payload.read();
				e = getEscapedVersion(c);
				if (e) {
					m_send.write('\\');
					m_send.write(e);
				} else {
					m_send.write(c);
				}
			}
		}

		// Add message tail
		if (!m_send.write('\n')) {
			rollback.rollback();
			return false;
		}
		return true;
	}

	// Extract a received message from the interconnect
	// Returns false if no waiting messages
	// Returns true if a message was extracted and placed in the 'out' parameters
	bool receive(Type& outHeader, ByteQueue& outPayload) {

		// Check if any messages are waiting
		if (m_receivedMessages == 0) return false;

		// Reset the output payload parameter before continuing
		outPayload.clear();

		// Get the message header
		uint8_t c = m_recv.read();
		if (c == '\n') {
			m_receivedMessages--;
			return false;
		}
		outHeader = Type(c);

		// Get the message body
		bool escaped = false;
		while (true) {

			// Get next character from buffer
			// If this is the tail character then exit here
			c = m_recv.read();
			if (c == '\n') {
				m_receivedMessages--;
				break;
			}

			// Check for escape status
			if (escaped) {
				escaped = false;
				outPayload.write(getNonEscapedVersion(c));
			} else if (c == '\\') {
				escaped = true;
			} else {
				outPayload.write(c);
			}
		}
		return true;
	}
	bool receive(Type& outHeader, String& outPayload) {
		ByteQueue queue;
		if (receive(outHeader, queue)) {
			outPayload = String();
			while (queue.available()) outPayload += (char)queue.read();
			return true;
		} else {
			return false;
		}
	}

	// Check if the receive buffer starts with the 'testString'
	// If the data matches then remove the data and return true
	// If the data does not match then return false
	// This function is mostly added just for the Bluetooth module
	bool compareAndReceive(const char* testString) {
		if (!testString) return false;

		// Loop through testString
		int i = 0;
		const int iMax = m_recv.available();
		while (true) {
			const char c = testString[i];
			if (!c) break; // End of testString
			if (c != (char)m_recv.peek(i)) return false; // Data mismatch
			if (++i >= iMax) return false; // End of receive buffer
		}
		if (i > 0) m_recv.discard(i);
		return true;
	}

	// Access receive buffer directly
	ByteQueue& receiveBuffer() {return m_recv;}

	// Access receive buffer directly
	const ByteQueue& receiveBuffer() const {return m_recv;}

	// Access send buffer directly
	ByteQueue& sendBuffer() {return m_send;}

	// Access send buffer directly
	const ByteQueue& sendBuffer() const {return m_send;}

	// Default constructor
	Interconnect(Stream& stream) :
		m_send(),
		m_recv(),
		m_receivedMessages(0),
		m_receivedAnything(false),
		m_stream(stream) {
	}

private:

	// Convert a character to its escaped version
	// The result character should follow a backslash when placed in a byte stream ('\n' for example)
	// Returns the escaped replacement for the input character, or zero if the character does not need to be escaped
	uint8_t getEscapedVersion(uint8_t character) {
		switch (character) {
			case '\\': return '\\';
			case '\n': return 'n';
			case '\r': return 'r';
			case 0:    return '0';
			default:   return 0;
		}
	}

	// Convert an escaped character to its non-escaped version
	// This input character should have been preceded by a backslash when taken from a byte stream ('\r' for example)
	uint8_t getNonEscapedVersion(uint8_t escapedCharacter) {
		switch (escapedCharacter) {
			case '\\': return '\\';
			case 'n':  return '\n';
			case 'r':  return '\r';
			case '0':  return 0;
			default:   return escapedCharacter;
		}
	}

	// Private data
	ByteQueue m_send;
	ByteQueue m_recv;
	uint8_t   m_receivedMessages;
	bool      m_receivedAnything;
	Stream&   m_stream;
};



#endif
