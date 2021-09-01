#ifndef TEST_MISC_H
#define TEST_MISC_H

// Include headers
#include "../Controller/misc.h"
#include "unit_test.h"

TEST(Misc) {
	using namespace Misc;

	// makeString()
	CHECK_EQUAL_SHOW(makeString("Hello", 2), "He");

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
