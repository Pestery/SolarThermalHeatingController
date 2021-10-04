#ifndef TEST_INTERCONNECT_H
#define TEST_INTERCONNECT_H

// Include headers
#include <cmath>
#include "arduino_stream.h"
#include "../Controller/interconnect.h"
#include "unit_test.h"

TEST(Interconnect) {
	Stream serial;
	Interconnect t(serial);
	Interconnect::Type header;
	String payload;

	// Setup test data
	const Interconnect::Type testHeader = Interconnect::Type::SendToDatabase;
	std::string testPayload = "TestDataBlah";
	std::string testMessage = (char)testHeader + testPayload + '\n';
	serial.debugSetReadBuffer(testMessage + testMessage);

	// Check initial conditions
	CHECK_IS_FALSE(t.receive(header, payload));
	CHECK_IS_TRUE(t.emptySendBuffer());
	CHECK_IS_TRUE(t.waitingMessages() == 0);

	// Run an update
	t.update();

	// Receive first message
	CHECK_EQUAL_SHOW(t.waitingMessages(), 2);
	CHECK_IS_TRUE(t.receive(header, payload));
	CHECK_EQUAL(header, testHeader);
	CHECK_EQUAL(payload, testPayload);

	// Receive second message
	CHECK_EQUAL_SHOW(t.waitingMessages(), 1);
	CHECK_IS_TRUE(t.receive(header, payload));
	CHECK_EQUAL(header, testHeader);
	CHECK_EQUAL(payload, testPayload);

	// Should not be a third message
	CHECK_EQUAL_SHOW(t.waitingMessages(), 0);
	CHECK_IS_FALSE(t.receive(header, payload));

	// Send a message
	CHECK_IS_TRUE(t.emptySendBuffer());
	CHECK_IS_TRUE(t.send(testHeader, testPayload));

	// Check message was sent to serial
	t.update();
	CHECK_EQUAL(testMessage, serial.debugGetWriteBuffer());
	serial.debugResetWriteBuffer();

	// Send a message with characters which need to be escaped
	testPayload = "Test\nHello\nWorld\n\rBlahBlah!!!";
	testMessage = (char)testHeader + std::string("Test\\nHello\\nWorld\\n\\rBlahBlah!!!") + '\n';
	t.send(testHeader, testPayload);
	t.update();
	CHECK_EQUAL_SHOW(testMessage, serial.debugGetWriteBuffer());
	serial.debugResetWriteBuffer();
}

#endif
