#ifndef TEST_BYTE_QUEUE_H
#define TEST_BYTE_QUEUE_H

// Include headers
#include "../Controller/byte_queue.h"
#include "unit_test.h"

TEST(ByteQueue) {

	ByteQueue t;
	uint8_t c;

	// Check initial condition
	CHECK_EQUAL_SHOW(t.available(), 0);
	CHECK_EQUAL_SHOW(t.read(), -1);
	CHECK_EQUAL_SHOW(t.peek(), -1);

	// Check adding single value
	CHECK_EQUAL(t.write('t'), 1);
	CHECK_EQUAL_SHOW(t.available(), 1);

	// Check removing single value
	c = t.read();
	CHECK_EQUAL_SHOW(t.available(), 0);
	CHECK_EQUAL_SHOW(t.read(), -1);
	CHECK_EQUAL_SHOW(t.peek(), -1);
	CHECK_EQUAL(c, 't');

	// Check adding multiple values sequentially
	// Fill the buffer
	for (int i=0; i<90; i++) {
		CHECK_EQUAL_SHOW(t.available(), i);
		CHECK_EQUAL(t.write(i + '!'), 1);
	}

	// Check peek() method
	for (int i=0; i<90; i++) {
		CHECK_EQUAL_SHOW(t.peek(i), i + '!');
	}

	// Check removing multiple values sequentially
	// Empty the buffer
	for (int i=0; i<90; i++) {
		CHECK_EQUAL_SHOW(t.available(), 90 - i);
		c = t.read();
		CHECK_EQUAL_SHOW(c, i + '!');
	}
	CHECK_EQUAL_SHOW(t.available(), 0);

	// Check adding a buffer of data
	{
		const char* s = "abcdefg";
		size_t i = strlen(s);
		size_t r = t.write((const uint8_t*)s, i);
		CHECK_EQUAL_SHOW(r, i);
	}
	CHECK_EQUAL_SHOW(t.peek(0), 'a');
	CHECK_EQUAL_SHOW(t.peek(1), 'b');
	CHECK_EQUAL_SHOW(t.peek(2), 'c');
	CHECK_EQUAL_SHOW(t.peek(3), 'd');
	CHECK_EQUAL_SHOW(t.peek(4), 'e');
	CHECK_EQUAL_SHOW(t.peek(5), 'f');
	CHECK_EQUAL_SHOW(t.peek(6), 'g');

	// Check discard() method
	CHECK_EQUAL_SHOW(t.available(), 7);
	t.discard(7);
	CHECK_EQUAL_SHOW(t.available(), 0);

	// Loop this section multiple times
	for (int r=0; r<40; r++) {

		// Add a small section of data
		for (int i=0; i<90; i++) {
			CHECK_EQUAL(t.write(i + '!'), 1);
		}

		// Remove a small section of data
		// Skip the first few loops to make sure the buffer is never empty
		if (r > 1) {
			for (int i=0; i<90; i++) {
				c = t.read();
				CHECK_EQUAL_SHOW(c, i + '!');
			}
		}
	}

	// Check clear() method
	CHECK_NOT_EQUAL(t.available(), 0);
	t.clear();
	CHECK_EQUAL_SHOW(t.available(), 0);

	// Check the roll-back functionality
	{
		for (int i=0; i<45; i++) {
			CHECK_EQUAL(t.write(i + '!'), 1);
		}
		CHECK_EQUAL_SHOW(t.available(), 45);
		ByteQueue::Rollback rb = t.createRollbackPoint();
		for (int i=45; i<90; i++) {
			CHECK_EQUAL(t.write(i + '!'), 1);
		}
		CHECK_EQUAL_SHOW(t.available(), 90);
		rb.rollback();
		CHECK_EQUAL_SHOW(t.available(), 45);
		for (int i=0; i<45; i++) {
			c = t.read();
			CHECK_EQUAL_SHOW(c, i + '!');
		}
		CHECK_EQUAL_SHOW(t.available(), 0);
	}

	// Check adding string to buffer
	CHECK_EQUAL(t.stringWrite("abcd"), 4);
	CHECK_EQUAL_SHOW(t.available(), 4);
	CHECK_EQUAL(t.read(), 'a');
	CHECK_EQUAL(t.read(), 'b');
	CHECK_EQUAL(t.read(), 'c');
	CHECK_EQUAL(t.read(), 'd');
	CHECK_EQUAL_SHOW(t.available(), 0);

	// Check stringCompareAndClearOnMatch()
	CHECK_EQUAL(t.stringWrite("HelloWorldTest"), 14);
	CHECK_EQUAL_SHOW(t.available(), 14);
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("World"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("HelloWrong"));
	CHECK_IS_TRUE(t.stringCompareAndClearOnMatch("Hello"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("Hello"));
	CHECK_EQUAL_SHOW(t.available(), 9);
	CHECK_IS_TRUE(t.stringCompareAndClearOnMatch("World"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("TestWrong"));
	CHECK_EQUAL_SHOW(t.available(), 4);
	CHECK_IS_TRUE(t.stringCompareAndClearOnMatch("Test"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("Test"));
	CHECK_EQUAL_SHOW(t.available(), 0);

	// Check stringIndexOf()
	CHECK_EQUAL(t.stringWrite("HelloWorldTest"), 14);
	CHECK_EQUAL_SHOW(t.stringIndexOf("Hello"), 0);
	CHECK_EQUAL_SHOW(t.stringIndexOf("World"), 5);
	CHECK_EQUAL_SHOW(t.stringIndexOf("Test"), 10);
	CHECK_EQUAL_SHOW(t.stringIndexOf("Wrong"), -1);
	CHECK_EQUAL_SHOW(t.stringIndexOf("l", 0), 2);
	CHECK_EQUAL_SHOW(t.stringIndexOf("l", 1), 2);
	CHECK_EQUAL_SHOW(t.stringIndexOf("l", 2), 2);
	CHECK_EQUAL_SHOW(t.stringIndexOf("l", 3), 3);
	CHECK_EQUAL_SHOW(t.stringIndexOf("l", 4), 8);
	CHECK_EQUAL_SHOW(t.stringIndexOf("l", 9), -1);
}

#endif
