#ifndef TEST_RING_BUFFER_H
#define TEST_RING_BUFFER_H

// Include headers
#include "../Controller/ring_buffer.h"
#include "unit_test.h"

TEST(RingBuffer) {

	RingBuffer t(16);
	uint8_t c;

	// Check initial condition
	CHECK_IS_TRUE(t.isEmpty());
	CHECK_IS_FALSE(t.isFull());
	CHECK_EQUAL_SHOW(t.freeSpace(), 15);
	CHECK_EQUAL_SHOW(t.size(), 0);

	// Check adding single value
	CHECK_IS_TRUE(t.push('t'));
	CHECK_IS_FALSE(t.isEmpty());
	CHECK_EQUAL_SHOW(t.freeSpace(), 14);
	CHECK_EQUAL_SHOW(t.size(), 1);

	// Check removing single value
	c = t.pop();
	CHECK_IS_TRUE(t.isEmpty());
	CHECK_EQUAL(t.freeSpace(), 15);
	CHECK_EQUAL(t.size(), 0);
	CHECK_EQUAL(c, 't');

	// Push and pop a few values to rotate the ring-buffer a bit, before subsequent tests
	for (int i=0; i<4; i++) {
		CHECK_IS_TRUE(t.push(i + 'm'));
		c = t.pop();
		CHECK_EQUAL(c, i + 'm');
	}

	// Check adding multiple values sequentially
	// Fill the buffer
	CHECK_IS_TRUE(t.isEmpty());
	for (int i=0; i<15; i++) {

		// Pre-add check
		CHECK_IS_FALSE(t.isFull());
		CHECK_EQUAL_SHOW(t.size(), i);
		CHECK_EQUAL_SHOW(t.freeSpace(), t.capacity() - t.size());

		// Add value
		CHECK_IS_TRUE(t.push(i + 'a'));

		// Post-add check
		CHECK_IS_FALSE(t.isEmpty());
		CHECK_EQUAL_SHOW(t.size(), i + 1);
		CHECK_EQUAL_SHOW(t.freeSpace(), t.capacity() - t.size());
	}
	CHECK_IS_TRUE(t.isFull());

	// Make sure cannot add extra values when buffer is full
	CHECK_EQUAL_SHOW(t.size(), 15);
	CHECK_IS_FALSE(t.push('z'));
	CHECK_IS_TRUE(t.isFull());
	CHECK_EQUAL_SHOW(t.size(), 15);
	CHECK_EQUAL_SHOW(t.freeSpace(), 0);

	// Check peek() methods
	CHECK_EQUAL(t.peek(), 'a');
	CHECK_EQUAL(t.peek(0), 'a');
	CHECK_EQUAL(t.peek(1), 'b');
	CHECK_EQUAL(t.peek(2), 'c');

	// Check removing multiple values sequentially
	// Empty the buffer
	CHECK_IS_TRUE(t.isFull());
	for (int i=0; i<15; i++) {

		// Pre-remove check
		CHECK_IS_FALSE(t.isEmpty());
		CHECK_EQUAL_SHOW(t.freeSpace(), i);
		CHECK_EQUAL_SHOW(t.size(), t.capacity() - t.freeSpace());

		// Remove value
		c = t.pop();
		CHECK_EQUAL(c, i + 'a');

		// Post-remove check
		CHECK_IS_FALSE(t.isFull());
		CHECK_EQUAL_SHOW(t.freeSpace(), i + 1);
		CHECK_EQUAL_SHOW(t.size(), t.capacity() - t.freeSpace());
	}
	CHECK_IS_TRUE(t.isEmpty());

	// Loop this section multiple times
	for (int r=0; r<40; r++) {

		// Add a small section of data
		for (int i=0; i<4; i++) {
			CHECK_EQUAL_SHOW(t.freeSpace(), t.capacity() - t.size());
			CHECK_IS_TRUE(t.push(i + 'a'));
			CHECK_EQUAL_SHOW(t.freeSpace(), t.capacity() - t.size());
		}
		CHECK_IS_FALSE(t.isFull());

		// Remove a small section of data
		// Skip the first few loops to make sure the buffer is never empty
		if (r > 1) {
			for (int i=0; i<4; i++) {
				CHECK_EQUAL_SHOW(t.freeSpace(), t.capacity() - t.size());
				CHECK_IS_FALSE(t.isEmpty());
				c = t.pop();
				CHECK_EQUAL(c, i + 'a');
				CHECK_EQUAL_SHOW(t.freeSpace(), t.capacity() - t.size());
			}
			CHECK_IS_FALSE(t.isEmpty());
		}
	}

	// Check reset() method
	t.reset();
	CHECK_IS_TRUE(t.isEmpty());
	CHECK_IS_FALSE(t.isFull());
	CHECK_EQUAL_SHOW(t.size(), 0);
	CHECK_EQUAL_SHOW(t.freeSpace(), 15);

	// Check minimum size buffer
	RingBuffer minBuf(0);
	CHECK_EQUAL_SHOW(minBuf.capacity(), 0);
	CHECK_IS_TRUE(minBuf.isFull());
	CHECK_IS_FALSE(minBuf.push('t'));

	// Check adding string to buffer
	t.resize(5);
	CHECK_EQUAL(t.stringPush("ab"), 2);
	CHECK_IS_FALSE(t.isEmpty());
	CHECK_EQUAL(t.pop(), 'a');
	CHECK_EQUAL(t.pop(), 'b');
	CHECK_IS_TRUE(t.isEmpty());

	CHECK_EQUAL(t.stringPush("abcdef"), 4);
	CHECK_IS_TRUE(t.isFull());
	CHECK_IS_FALSE(t.isEmpty());
	CHECK_EQUAL(t.pop(), 'a');
	CHECK_EQUAL(t.pop(), 'b');
	CHECK_EQUAL(t.pop(), 'c');
	CHECK_EQUAL(t.pop(), 'd');
	CHECK_IS_TRUE(t.isEmpty());

	// Check stringCompareAndClearOnMatch()
	t.resize(16);
	CHECK_EQUAL(t.stringPush("HelloWorldTest"), 14);
	CHECK_IS_FALSE(t.isEmpty());
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("World"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("HelloWrong"));
	CHECK_IS_TRUE(t.stringCompareAndClearOnMatch("Hello"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("Hello"));
	CHECK_EQUAL_SHOW(t.size(), 9);
	CHECK_IS_TRUE(t.stringCompareAndClearOnMatch("World"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("TestWrong"));
	CHECK_EQUAL_SHOW(t.size(), 4);
	CHECK_IS_TRUE(t.stringCompareAndClearOnMatch("Test"));
	CHECK_IS_FALSE(t.stringCompareAndClearOnMatch("Test"));
	CHECK_IS_TRUE(t.isEmpty());

	// Check stringIndexOf()
	CHECK_EQUAL(t.stringPush("HelloWorldTest"), 14);
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
