#ifndef INTERCONNECT_H
#define INTERCONNECT_H

// Bit-rate for Arduino-ESP8266 serial communication
#define SERIAL_BITRATE_ARDUINO_ESP8266 74880

// Bit-rate for Arduino-PC serial communication
#define SERIAL_BITRATE_ARDUINO_PC 9600

// Used to make communication between the Arduino and ESP8266 a bit easier
class Interconnect {
public:

	// A list of possible command types
	enum Type : uint8_t {

		// A small request to allow a DataForDatabase command to be sent
		RequestSendDataForDatabase = 'r',

		// A small acknowledgement in reply to RequestSendDataForDatabase, and allowing a DataForDatabase command to be sent
		AllowSendDataForDatabase = 'a',

		// The header for a large message which contains data destined for the database
		DataForDatabase  = 'd',

		// A notification message which should be forwarded to the user, probably via the serial port
		GeneralNotification = 'N',

		// A debug command used to echo back data to the source
		EchoArduino = 'x',
		EchoESP8266 = 'y',

		// Used to indicate an error with the interconnect
		TypeError = 0
	};

	// Check if there is any data waiting to be sent
	inline bool sendRequired() const __attribute__((always_inline)) {return m_sendLength > m_sendOffset;}

	// Check if the send buffer is empty
	// If false then there is data waiting to be sent
	inline bool emptySendBuffer() const __attribute__((always_inline)) {return m_sendLength <= m_sendOffset;}

	// Get the next byte to send
	// This will remove the byte from the send queue
	uint8_t getNextByteToSend() {

		// Check if there is anything to send
		// If not then return the default character
		if (m_sendOffset >= m_sendLength) return '\n';

		// Get next byte to send
		// Increment offset for next time
		uint8_t result = m_sendBuffer[m_sendOffset++];

		// Check if the buffer is empty
		// If so then delete buffer
		if (m_sendOffset >= m_sendLength) {
			free(m_sendBuffer);
			m_sendBuffer = nullptr;
			m_sendAllocated = 0;
			m_sendLength = 0;
			m_sendOffset = 0;
		}

		/* Serial.println();
		Serial.print("Next byte to send: ");
		if (result > 32) Serial.println((char)result); else Serial.println(String('[') + result + String(']'));
		debugDump(); //*/

		// Finish up
		return result;
	}

	// Reset send buffer
	// This will replace all content in the send buffer with a single byte reset marker
	// When the other end receives the reset marker it will clear its receive buffer as well
	// Returns true on success, or false if not enough memory was available
	// TODO: This might need to be changed. Needs to be called if the serial buffer overflows, which is on the receive end, not send...?
	bool reset(Type header, const String& payload = String()) {

		// If a buffer exists then delete it
		if (m_sendBuffer) free(m_sendBuffer);

		// Create a new buffer
		// Add the reset marker to the buffer
		m_sendAllocated = 1;
		m_sendBuffer = (uint8_t*)malloc(m_sendAllocated);
		if (!m_sendBuffer) return false;
		m_sendLength = 1;
		m_sendOffset = 0;
		m_sendBuffer[0] = '\r';

		// Finish up
		return true;
	}

	// Output the internal state to Serial
	void debugDump() {
		Serial.print(F("m_sendLength=    ")); Serial.println(m_sendLength);
		Serial.print(F("m_sendAllocated= ")); Serial.println(m_sendAllocated);
		Serial.print(F("m_sendOffset=    ")); Serial.println(m_sendOffset);
		Serial.print(F("m_recvLength=    ")); Serial.println(m_recvLength);
		Serial.print(F("m_recvAllocated= ")); Serial.println(m_recvAllocated);

		Serial.print(F("m_sendBuffer=    "));
		if (m_sendBuffer) {
			for (int i=0; i<m_sendLength; i++) Serial.write(m_sendBuffer[i]);
		}
		Serial.println();

		Serial.print(F("m_recvBuffer=    "));
		if (m_recvBuffer) {
			for (int i=0; i<m_recvLength; i++) Serial.write(m_recvBuffer[i]);
		}
		Serial.println();

		Serial.print(F("m_recvEscapeNext=")); Serial.println(m_recvEscapeNext);
	}

	// Add data to the send queue
	// Avoid adding new data to the queue if it already contains some unsent data
	// Returns true on success, or false if not enough memory was available
	bool send(Type header, const String& payload = String()) {

		// Debug output to serial
		/*
		#ifdef MAIN_ARDUINO_H
		Serial.print((char)header);
		Serial.print(':');
		Serial.println(payload);

		debugDump();
		#endif //*/



		// Get length of payload
		const uint16_t payloadLength = payload.length();

		// Check if a buffer exists
		if (!m_sendBuffer) {

			// Create new buffer
			m_sendAllocated = payloadLength + 2;
			m_sendBuffer = (uint8_t*)malloc(m_sendAllocated);
			if (!m_sendBuffer) return false;
			m_sendLength = 0;
			m_sendOffset = 0;

		} else {

			// Get the total required size (new content plus existing)
			// Resize the buffer if required
			uint16_t newAllocated = (m_sendLength - m_sendOffset) + (payloadLength + 2);
			if (newAllocated > m_sendAllocated) {
				if (!resizeSendBuffer(newAllocated)) return false;
			}

			// Move any existing data to the front of the buffer
			if (m_sendOffset > 0) {
				for (uint16_t i=0, j=m_sendOffset; j<m_sendLength; i++, j++) m_sendBuffer[i] = m_sendBuffer[j];
				m_sendLength -= m_sendOffset;
				m_sendOffset = 0;
			}
		}

		// Declare working variables
		// This will allow reverting back if an error occurs
		uint16_t newLength = m_sendLength;

		// Add header
		// The earlier code will have ensured that there is at least enough space for the header
		m_sendBuffer[newLength++] = header;

		// Add payload
		for (int i=0; i<payloadLength; i++) {

			// Escape any newline characters and backslashes
			char c = payload[i];
			switch (c) {
				case '\n':
					if (!reserveFreeSpaceSendBuffer(2)) return false;
					m_sendBuffer[newLength++] = '\\';
					m_sendBuffer[newLength++] = 'n';
					break;
				case '\\':
					if (!reserveFreeSpaceSendBuffer(2)) return false;
					m_sendBuffer[newLength++] = '\\';
					m_sendBuffer[newLength++] = '\\';
					break;
				case '\r':
					if (!reserveFreeSpaceSendBuffer(2)) return false;
					m_sendBuffer[newLength++] = '\\';
					m_sendBuffer[newLength++] = '\r';
					break;
				default:
					if (!reserveFreeSpaceSendBuffer(1)) return false;
					m_sendBuffer[newLength++] = c;
					break;
			}
		}

		// Add tail
		resizeSendBuffer(newLength + 1);
		m_sendBuffer[newLength++] = '\n';

		// Finish up
		m_sendLength = newLength;
		return true;
	}

	// Add a byte to the receive buffer
	// Returns true if enough data has been received to get a message, or false if no message is ready yet
	bool receiveByte(int receivedByte) {
		if (receivedByte == -1) return false;

		/*
		Serial.print("received: ");
		if (receivedByte > 32) {
			Serial.println((char)receivedByte);
		} else {
			Serial.print("[");
			Serial.print(receivedByte);
			Serial.println(']');
		} //*/

		// Check if a buffer exists
		if (!m_recvBuffer) {

			// Create new buffer
			m_recvAllocated = 32;
			m_recvBuffer = (uint8_t*)malloc(m_recvAllocated);
			if (!m_recvBuffer) return false;
			m_recvLength = 0;
			m_recvEscapeNext = false;

		} else if (m_recvLength == m_recvAllocated) {

			// Buffer is full
			// Make it larger
			resizeRecvBuffer(m_recvAllocated + 32);
		}

		// Add the data to the buffer
		// Check if this character needs to be escaped
		if (m_recvEscapeNext) {
			m_recvEscapeNext = false;

			// Add the character to the buffer in its original form
			switch (receivedByte) {
				case 'n':
					m_recvBuffer[m_recvLength++] = '\n';
					break;
				case 'r':
					m_recvBuffer[m_recvLength++] = '\r';
					break;
				default:
					m_recvBuffer[m_recvLength++] = (uint8_t)receivedByte;
					break;
			}

		} else if (receivedByte == '\\') {

			// Mark the next character as needing to be escaped
			m_recvEscapeNext = true;

		} else if (receivedByte == '\n') {

			// Received end-of-message marker
			// Return true if the message thus far is valid
			return m_recvLength > 0;

		} else if (receivedByte == '\r') {

			// Received reset message
			// An error must have occurred at the other end, so reset the receive buffer
			free(m_recvBuffer);
			m_recvBuffer = nullptr;
			m_recvAllocated = 0;
			m_recvLength = 0;
			return false;

		} else {

			// If here then add the received character to the receive buffer
			m_recvBuffer[m_recvLength++] = (uint8_t)receivedByte;
		}
		return false;
	}

	// Get the type of message being received
	Type getReceivedType() const {
		return m_recvBuffer ? Type(m_recvBuffer[0]) : Type::TypeError;
	}

	// Get the message from the receive buffer
	// This will remove the message from the receive buffer
	String getReceivedMessage() {

		// Generate result
		String result;
		if (m_recvLength > 1) {
			result.reserve(m_recvLength - 1);
			for (int i=1; i<m_recvLength; i++) result += (char)m_recvBuffer[i];
		}

		// Remove data from buffer
		free(m_recvBuffer);
		m_recvBuffer = nullptr;
		m_recvAllocated = 0;
		m_recvLength = 0;

		// Return message
		return result;
	}

	// Default constructor
	Interconnect() :
		m_sendLength(0),
		m_sendAllocated(0),
		m_sendOffset(0),
		m_recvLength(0),
		m_recvAllocated(0),
		m_sendBuffer(nullptr),
		m_recvBuffer(nullptr),
		m_recvEscapeNext(false) {
	}

private:

	// Try to change the size of the send buffer
	// Returns true on success, or false if a memory allocation error occurred
	bool resizeSendBuffer(uint16_t newAllocated) {
		if (newAllocated == m_sendAllocated) {
			return true;
		} else {
			uint8_t* newBuffer = (uint8_t*)realloc(m_sendBuffer, newAllocated);
			if (newBuffer) {
				m_sendBuffer = newBuffer;
				m_sendAllocated = newAllocated;
				return true;
			} else {
				return false;
			}
		}
	}

	// Reserve an amount of free space within the send buffer
	// Returns true on success, or false if a memory allocation error occurred
	inline bool reserveFreeSpaceSendBuffer(uint16_t requiredSpace) __attribute__((always_inline)) {
		requiredSpace += m_sendLength;
		if (m_sendAllocated < requiredSpace) {
			if (!resizeSendBuffer(requiredSpace + 8)) return false;
		}
		return true;
	}

	// Try to change the size of the receive buffer
	// Returns true on success, or false if a memory allocation error occurred
	bool resizeRecvBuffer(uint16_t newAllocated) {
		if (newAllocated == m_recvAllocated) {
			return true;
		} else {
			uint8_t* newBuffer = (uint8_t*)realloc(m_recvBuffer, newAllocated);
			if (newBuffer) {
				m_recvBuffer = newBuffer;
				m_recvAllocated = newAllocated;
				return true;
			} else {
				return false;
			}
		}
	}

	// Private data
	uint16_t m_sendLength;    // Amount of data in the send buffer
	uint16_t m_sendAllocated; // Amount of memory allocated for the send buffer
	uint16_t m_sendOffset;    // Offset of the next byte to be sent

	uint16_t m_recvLength;    // Amount of data in the receive buffer
	uint16_t m_recvAllocated; // Amount of memory allocated for the receive buffer

	uint8_t* m_sendBuffer; // Send buffer
	uint8_t* m_recvBuffer; // Receive buffer

	bool m_recvEscapeNext; // Escape the next received byte
};



#endif
