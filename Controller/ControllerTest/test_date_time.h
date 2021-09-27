#ifndef TEST_DATE_TIME_H
#define TEST_DATE_TIME_H

// Include headers
#include "arduino_pins.h"
#include "arduino_string.h"
#include "../Controller/date_time.h"
#include "unit_test.h"

TEST(DateTime) {
	DateTime t, s;

	t = 12345;
	CHECK_EQUAL(t, 12345);

	t = "123456";
	CHECK_EQUAL(t, 123456);
	//CHECK_EQUAL(t.toString(), "123456")

	t = 1;
	s = t;
	CHECK_EQUAL(t, s);

	t = t + s;
	CHECK_EQUAL(t, 2);
}

#endif
