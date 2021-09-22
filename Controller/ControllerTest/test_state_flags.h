#ifndef TEST_STATE_FLAGS_H
#define TEST_STATE_FLAGS_H

// Include headers
#include "../Controller/state_flags.h"
#include "unit_test.h"

TEST(StateFlags) {

	constexpr StateFlags::Type f1 = 1 << 0;
	constexpr StateFlags::Type f2 = 1 << 1;
	constexpr StateFlags::Type f3 = 1 << 2;

	StateFlags t(f1 | f2);

	CHECK_IS_TRUE(t.anySet(f1))
	CHECK_IS_TRUE(t.anySet(f1 | f2))
	CHECK_IS_TRUE(t.anySet(f1 | f2 | f3))

	CHECK_IS_TRUE(t.isSet(f1))
	CHECK_IS_TRUE(t.isSet(f1 | f2))
	CHECK_IS_FALSE(t.isSet(f1 | f2 | f3))

	t.set(f3);
	CHECK_IS_TRUE(t.isSet(f1 | f2))
	CHECK_IS_TRUE(t.isSet(f1 | f2 | f3))

	t.reset(f3);
	CHECK_IS_TRUE(t.isSet(f1 | f2))
	CHECK_IS_FALSE(t.isSet(f1 | f2 | f3))
}

#endif
