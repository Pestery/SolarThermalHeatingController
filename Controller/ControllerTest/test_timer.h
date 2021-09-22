#ifndef TEST_TIMER_H
#define TEST_TIMER_H

// Include headers
#include "../Controller/timer.h"
#include "unit_test.h"

TEST(Timer) {
	Timer t(10);

	// First tick
	CHECK_IS_FALSE(t.triggered(9));
	CHECK_IS_TRUE(t.triggered(10));
	CHECK_IS_FALSE(t.triggered(11));

	// Second tick
	CHECK_IS_FALSE(t.triggered(19));
	CHECK_IS_TRUE(t.triggered(20));
	CHECK_IS_FALSE(t.triggered(21));

	// Reset test
	t.reset(20);
	CHECK_IS_FALSE(t.triggered(19));
	CHECK_IS_TRUE(t.triggered(20));

	// Loop around test
	CHECK_IS_TRUE(t.triggered(19));
}

#endif
