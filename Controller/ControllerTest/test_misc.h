#ifndef TEST_MISC_H
#define TEST_MISC_H

// Include headers
#include "../Controller/misc.h"
#include "unit_test.h"

TEST(Misc) {
	using namespace Misc;

	// Min()
	CHECK_EQUAL(min(1, 2), 1);
	CHECK_EQUAL(min(2.0, 1.8), 1.8);

	// Max()
	CHECK_EQUAL(max(1, 2), 2);
	CHECK_EQUAL(max(2.0, 1.8), 2.0);

	// Clamp()
	CHECK_EQUAL(clamp(1.5, 1.0, 2.0), 1.5);
	CHECK_EQUAL(clamp(1.0, 1.0, 2.0), 1.0);
	CHECK_EQUAL(clamp(2.0, 1.0, 2.0), 2.0);
	CHECK_EQUAL(clamp(0.5, 1.0, 2.0), 1.0);
	CHECK_EQUAL(clamp(2.5, 1.0, 2.0), 2.0);

	// getNextSubString()
	String source = " Hello, World,, , Blah";
	int startAt = 0;
	CHECK_EQUAL(" Hello", getNextSubString(source, startAt, ','));
	CHECK_EQUAL(" World", getNextSubString(source, startAt, ','));
	CHECK_EQUAL("", getNextSubString(source, startAt, ','));
	CHECK_EQUAL(" ", getNextSubString(source, startAt, ','));
	CHECK_EQUAL(" Blah", getNextSubString(source, startAt, ','));

}

#endif
