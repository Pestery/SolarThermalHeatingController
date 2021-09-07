#ifndef TEST_MISC_H
#define TEST_MISC_H

// Include headers
#include "../Controller/misc.h"
#include "unit_test.h"

TEST(Misc) {
	using namespace Misc;

	// makeString()
	CHECK_EQUAL_SHOW(makeString("Hello", 2), "He");

	// hexToInt()
	CHECK_EQUAL_SHOW(hexToInt("5"), 5);
	CHECK_EQUAL_SHOW(hexToInt("f"), 0xf);
	CHECK_EQUAL_SHOW(hexToInt("11"), 0x11);
	CHECK_EQUAL_SHOW(hexToInt("ee"), 0xee);
	CHECK_EQUAL_SHOW(hexToInt("ee e"), 0xee);
	CHECK_EQUAL_SHOW(hexToInt("eege"), 0xee);
	CHECK_EQUAL_SHOW(hexToInt("0x5"), 5);
	CHECK_EQUAL_SHOW(hexToInt("0X5"), 5);
	CHECK_EQUAL_SHOW(hexToInt("0xf"), 0xf);
	CHECK_EQUAL_SHOW(hexToInt("0x11"), 0x11);
	CHECK_EQUAL_SHOW(hexToInt("0xee"), 0xee);
	CHECK_EQUAL_SHOW(hexToInt("0xee e"), 0xee);
	CHECK_EQUAL_SHOW(hexToInt("0xeege"), 0xee);

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
