#ifndef BLUETOOTH_INTERCONNECT_H
#define BLUETOOTH_INTERCONNECT_H

// Include headers
#include "interconnect.h"

// The Bluetooth class holds information about a date and time.
class BluetoothInterconnect {
public:

	// Returns true if the Bluetooth is connected
	bool isConnected() {
		return m_connected;
	}

	// Update the Bluetooth interconnect
	// This will send and receive any waiting data in the related buffers
	void update() {

		// Update the interconnect
		// This will send and receive any pending data
		m_interconnect.update();

		// Enter a loop to check for messages from the Bluetooth module
		if (m_interconnect.receivedAnything()) {
			while (m_interconnect.receiveBuffer().available()) {

				// Any messages from the Bluetooth module will start with "OK"
				if (m_interconnect.receiveBuffer().peek() == 'O') {

					// The first letter in the buffer is an 'O', which might be the start of a Bluetooth module message
					// First check if the Bluetooth is connected
					// Then check which, if any, commands are found
					if (m_connected) { // Connected
						if (compareAndClearOnMatch("OK+LOST")) {

							// Bluetooth connection lost message
							// This occurs when the connected device disconnects from the Bluetooth module
							m_connected = false;
							m_interconnect.sendBuffer().clear();

						} else {

							// If here then the Bluetooth is connected and no commands are recognised
							// So this is probably data sent to the Arduino
							// Exit the loop
							break;

						}
					} else { // Not connected
						if (compareAndClearOnMatch("OK+CONN")) {

							// Bluetooth connected message
							// This is when something connects to the Bluetooth module
							m_connected = true;
							m_interconnect.sendBuffer().clear();

						} else {

							// If here then the Bluetooth is not connected
							// The first byte in the receive buffer is an 'O', but is not a recognised command
							// Check if an "OK" exists later in the buffer, and if so then clear everything to that point
							int i = findNextInReceiveBuffer("OK", 1);
							if (i > 0) {
								m_interconnect.receiveBuffer().discard(i);
							} else {
								break;
							}
						}
					}
				} else if (m_connected) {

					// If here then the Bluetooth is connected and the first byte in the receive buffer is not an 'O'
					// So this is probably data sent to the Arduino
					// Exit the loop
					break;

				} else {

					// If here then the Bluetooth is not connected and the first byte in the receive buffer is not an 'O'
					// So clear any receive data because it is should just be junk from the Bluetooth module
					m_interconnect.receiveBuffer().read();
				}
			}
		}
	}

	// Add data to the send queue
	// Returns true on success, or false if there was not enough space in the buffer
	bool send(Interconnect::Type header) {
		return send(header, nullptr);
	}
	bool send(Interconnect::Type header, const String& payload) {
		return send(header, payload.c_str());
	}
	bool send(Interconnect::Type header, const char* payload) {

		// Only send if connected
		// If not connected then just return true
		return m_connected ? m_interconnect.send(header, payload) : true;
	}
	bool send(Interconnect::Type header, ByteQueue& payload) {

		// Only send if connected
		// If not connected then just return true
		return m_connected ? m_interconnect.send(header, payload) : true;
	}

	// Extract a received message from the interconnect
	// Returns false if no waiting messages
	// Returns true if a message was extracted and placed in the 'out' parameters (may block if message is longer than receive buffer)
	bool receive(Interconnect::Type& outHeader, String& outPayload) {

		// Only receive if connected
		return m_connected ? m_interconnect.receive(outHeader, outPayload) : false;
	}
	bool receive(Interconnect::Type& outHeader, ByteQueue& outPayload) {

		// Only receive if connected
		return m_connected ? m_interconnect.receive(outHeader, outPayload) : false;
	}

	// Check if the receive buffer starts with the 'testString'
	// If the data matches then remove the data and return true
	// If the data does not match then return false
	// This function is mostly added just for the Bluetooth module
	bool compareAndClearOnMatch(const char* testString) {
		return m_interconnect.receiveBuffer().stringCompareAndClearOnMatch(testString);
	}

	// Find next occurrence of the given string within the receive buffer
	// Returns the offset of the start of the string within the buffer, or -1 if not found
	int findNextInReceiveBuffer(const char* testString, int startAt = 0) {
		return m_interconnect.receiveBuffer().stringIndexOf(testString, startAt);
	}

	// Constructor
	BluetoothInterconnect(Stream& stream) :
		m_connected(false),
		m_interconnect(stream) {
	}

private:
	bool m_connected;
	Interconnect m_interconnect; // Unix time stamp. Number of seconds since 1/1/1970 at UTC
};

#endif
