#ifndef INTERCONNECT_H
#define INTERCONNECT_H

// Include headers
#include "ring_buffer.h"

// Bit-rate for Arduino-ESP8266 serial communication
#define SERIAL_BITRATE_ARDUINO_ESP8266 74880

// Bit-rate for Arduino-PC serial communication
#define SERIAL_BITRATE_ARDUINO_PC 9600

// Interconnect buffer sizes for different connections and direction of connections
// This should be set to reflect the expected maximum size of message to be sent on that link
#define INTERCONNECT_BUFFER_ARDUINO_TO_ESP8266   250
#define INTERCONNECT_BUFFER_ARDUINO_TO_BLUETOOTH 32
#define INTERCONNECT_BUFFER_ARDUINO_TO_PC        32
#define INTERCONNECT_BUFFER_ESP8266_TO_ARDUINO   64
#define INTERCONNECT_BUFFER_BLUETOOTH_TO_ARDUINO 32
#define INTERCONNECT_BUFFER_PC_TO_ARDUINO        32

// Used to make communication between the Arduino and ESP8266 a bit easier
class Interconnect {
public:

	// A list of possible command types
	// Note: An upper-case 'O' must not be used for any command letter, as it is used for Bluetooth module messages
	enum Type : uint8_t {

		// A small request to allow a DataForDatabase command to be sent
		RequestSendDataForDatabase = 'r',

		// A small acknowledgement in reply to RequestSendDataForDatabase, and allowing a DataForDatabase command to be sent
		AllowSendDataForDatabase = 'a',

		// The header for a large message which contains data destined for the database
		DataForDatabase  = 'd',

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

		// Set the wifi SSID name and password
		// The payload should be either:
		//  - The wifi name
		//  - The wifi name, followed by plus '+' symbol, followed by wifi password
		SetWifiInfo = 'w',

		// Request the wifi status
		// No payload
		GetWifiInfo = 'W',

		// A debug command used to send data to server
		// The response will be forwarded back through to the serial port
		// Payload is the (probably JSON) message to be sent to the server
		DebugSendToServerKeepHeaders = 'T',
		DebugSendToServer = 't',

		// A debug command used to echo back data to the source
		// Payload is the test message
		EchoArduino = 'x',
		EchoESP8266 = 'y',

		// Used to indicate no message
		None = 0
	};

	// Update the interconnect
	// This will send and receive any waiting data in the related buffers
	void update() {

		// Receive data
		while (m_stream.available() && !m_recv.isFull()) {
			uint8_t c = m_stream.read();
			m_recv.push(c);
			if (c == '\n') m_receivedMessages++;
			m_receivedAnything = true;
		}

		// Send data
		while (!m_send.isEmpty() && (m_stream.availableForWrite() > 0)) {
			m_stream.write(m_send.pop());
		}
	}

	// Returns true if the receive buffer is full and should be emptied
	int fullReceiveBuffer() const {
		return m_recv.isFull();
	}

	// Return true if the send buffer is empty
	int emptySendBuffer() const {
		return m_send.isEmpty();
	}

	// Returns true if there are any received messages within the receive buffer
	int waitingMessages() const {
		return (m_receivedMessages > 0);
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

		// Get the length of the payload string
		const int payloadLength = payload ? strlen(payload) : 0;

		// Count the number of characters which need to be escaped
		int escapedCharacters = 0;
		for (int i=0; i<payloadLength; i++) {
			if (getEscapedVersion(payload[i])) escapedCharacters++;
		}

		// Make sure there is enough space in the send buffer
		if (m_send.freeSpace() < (2 + payloadLength + escapedCharacters)) {
			return false;
		} else {

			// Add message header
			m_send.push(header);

			// Add message body, if any
			uint8_t c, e;
			for (int i=0; i<payloadLength; i++) {
				c = payload[i];
				e = getEscapedVersion(c);
				if (e) {
					m_send.push('\\');
					m_send.push(e);
				} else {
					m_send.push(c);
				}
			}

			// Add message tail
			m_send.push('\n');
			return true;
		}
	}

	// Add data to the send queue
	// This method will always send the message, but may block if the buffer is full
	void sendForce(Type header) {
		sendForce(header, nullptr);
	}
	void sendForce(Type header, const String& payload) {
		sendForce(header, payload.c_str());
	}
	void sendForce(Type header, const char* payload) {

		// Send whatever data can be sent
		while (!m_send.isEmpty() && (m_stream.availableForWrite() > 0)) {
			m_stream.write(m_send.pop());
		}

		// Add message header
		while (!m_send.push(header)) m_stream.write(m_send.pop());

		// Add message body, if any
		uint8_t c, e;
		const int payloadLength = payload ? strlen(payload) : 0;
		for (int i=0; i<payloadLength; i++) {
			c = payload[i];
			e = getEscapedVersion(c);
			if (e) {
				while (!m_send.push('\\')) m_stream.write(m_send.pop());
				while (!m_send.push(e)) m_stream.write(m_send.pop());
			} else {
				while (m_send.freeSpace() < 1) m_stream.write(m_send.pop());
				while (!m_send.push(c)) m_stream.write(m_send.pop());
			}
		}

		// Add message tail
		while (!m_send.push('\n')) m_stream.write(m_send.pop());
	}

	// Extract a received message from the interconnect
	// Returns false if no waiting messages
	// Returns true if a message was extracted and placed in the 'out' parameters (may block if message is longer than receive buffer)
	bool receive(Type& outHeader, String& outPayload) {

		// Check if any messages are waiting
		// Also check if the receive buffer is full
		if ((m_receivedMessages == 0) && !m_recv.isFull()) return false;

		// Reset the output payload parameter before continuing
		outPayload = String();

		// Get the message header
		uint8_t c = m_recv.pop();
		if (c == '\n') {
			m_receivedMessages--;
			return false;
		}
		outHeader = Type(c);

		// Find the end marker and get the expected length of payload
		int payloadLength = 0;
		while ((payloadLength < m_recv.size()) && (m_recv.peek(payloadLength) != '\n')) payloadLength++;

		// Get the message body
		if (payloadLength > 0) {
			bool escaped = false;
			outPayload.reserve(payloadLength);
			while (true) {

				// Check if the receive buffer is empty
				// If so then wait for more data to come in, because the end of this message has not been reached yet
				while (m_recv.isEmpty()) update();

				// Get next character from buffer
				// If this is the tail character then exit here
				c = m_recv.pop();
				if (c == '\n') {
					m_receivedMessages--;
					break;
				}

				// Check for escape status
				if (escaped) {
					escaped = false;
					outPayload += (char)getNonEscapedVersion(c);
				} else if (c == '\\') {
					escaped = true;
				} else {
					outPayload += (char)c;
				}
			}
		}
		return true;
	}

	// Check if the receive buffer starts with the 'testString'
	// If the data matches then remove the data and return true
	// If the data does not match then return false
	// This function is mostly added just for the Bluetooth module
	bool compareAndReceive(const char* testString) {
		if (!testString) return false;

		// Loop through testString
		int i = 0;
		const int iMax = m_recv.size();
		while (true) {
			const char c = testString[i];
			if (!c) break; // End of testString
			if (c != (char)m_recv.peek(i)) return false; // Data mismatch
			if (++i >= iMax) return false; // End of receive buffer
		}
		return true;
	}

	// Access receive buffer directly
	RingBuffer& receiveBuffer() {return m_recv;}

	// Access receive buffer directly
	const RingBuffer& receiveBuffer() const {return m_recv;}

	// Access send buffer directly
	RingBuffer& sendBuffer() {return m_send;}

	// Access send buffer directly
	const RingBuffer& sendBuffer() const {return m_send;}

	// Default constructor
	Interconnect(Stream& stream, RingBuffer::IndexType sendBufferSize, RingBuffer::IndexType recvBufferSize) :
		m_send(sendBufferSize),
		m_recv(recvBufferSize),
		m_receivedMessages(0),
		m_receivedAnything(false),
		m_stream(stream) {
	}

private:

	// Convert a character to its escaped version
	// The result character should follow a backslash when placed in a byte stream ('\r' for example)
	// Returns the escaped replacement for the input character, or zero if the character does not need to be escaped
	uint8_t getEscapedVersion(uint8_t character) {
		switch (character) {
			case '\\': return '\\';
			case '\n': return 'n';
			case '\r': return 'r';
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
			default:   return escapedCharacter;
		}
	}

	// Private data
	RingBuffer m_send;
	RingBuffer m_recv;
	uint8_t    m_receivedMessages;
	bool       m_receivedAnything;
	Stream& m_stream;
};



#endif
