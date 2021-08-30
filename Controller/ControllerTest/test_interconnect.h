#ifndef TEST_INTERCONNECT_H
#define TEST_INTERCONNECT_H

// Include headers
#include <cmath>
#include "arduino_hardware_serial.h"
#include "../Controller/interconnect.h"
#include "unit_test.h"

TEST(Interconnect) {
	HardwareSerial serial;
	Interconnect t(serial, 16, 16);
	Interconnect::Type header;
	String payload;

	// Setup test data
	const Interconnect::Type testHeader = Interconnect::Type::DataForDatabase;
	const std::string testPayload = "TestDataBlah";
	const std::string testMessage = (char)testHeader + testPayload + '\n';
	serial.debugSetReadBuffer(testMessage + testMessage);

	// Check initial conditions
	CHECK_IS_FALSE(t.receive(header, payload));
	CHECK_IS_TRUE(t.emptySendBuffer());

	// Run an update
	t.update();

	// Check that receive buffer is full
	CHECK_IS_TRUE(t.emptySendBuffer());
	CHECK_IS_TRUE(t.fullReceiveBuffer());
	CHECK_EQUAL_SHOW(t.waitingMessages(), 1);

	// Receive a message
	CHECK_IS_TRUE(t.receive(header, payload));
	CHECK_EQUAL(header, testHeader);
	CHECK_EQUAL(payload, testPayload);

	// Receive second message
	payload = String();
	CHECK_IS_FALSE(t.receive(header, payload));
	CHECK_EQUAL(t.waitingMessages(), 0);
	t.update();
	CHECK_EQUAL(t.waitingMessages(), 1);
	CHECK_IS_TRUE(t.receive(header, payload));
	CHECK_EQUAL(header, testHeader);
	CHECK_EQUAL(payload, testPayload);

	// Send a message
	CHECK_IS_TRUE(t.emptySendBuffer());
	CHECK_IS_TRUE(t.send(testHeader, testPayload)); // One message should fit
	CHECK_IS_FALSE(t.emptySendBuffer());
	CHECK_IS_FALSE(t.send(testHeader, testPayload)); // Two messages should not fit
	CHECK_IS_FALSE(t.emptySendBuffer());

	// Check message was sent to serial
	t.update();
	CHECK_EQUAL(testMessage, serial.debugGetWriteBuffer());
	serial.debugResetWriteBuffer();

	// Send three messages and make sure they are sent
	t.sendForce(testHeader, testPayload);
	t.sendForce(testHeader, testPayload);
	t.sendForce(testHeader, testPayload);
	t.update();
	CHECK_EQUAL(testMessage + testMessage + testMessage, serial.debugGetWriteBuffer());
}

#endif
