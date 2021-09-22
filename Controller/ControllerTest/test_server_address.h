#ifndef TEST_SERVER_ADDRESS_H
#define TEST_SERVER_ADDRESS_H

// Include headers
#include "../Controller/server_address.h"
#include "unit_test.h"

TEST(ServerAddress) {

	ServerAddress t;

	CHECK_IS_TRUE(t.set(String("http://host:1234/path")));
	CHECK_IS_FALSE(t.secure());
	CHECK_EQUAL_SHOW(t.host(), String("host"));
	CHECK_EQUAL_SHOW(t.port(), 1234);
	CHECK_EQUAL_SHOW(t.path(), String("/path"));

	CHECK_IS_TRUE(t.set(String("https://host:1/path")));
	CHECK_IS_TRUE(t.secure());
	CHECK_EQUAL_SHOW(t.host(), String("host"));
	CHECK_EQUAL_SHOW(t.port(), 1);
	CHECK_EQUAL_SHOW(t.path(), String("/path"));

	CHECK_IS_TRUE(t.set(String("https://localHost:43210/partOne/PartTwo/partThree")));
	CHECK_IS_TRUE(t.secure());
	CHECK_EQUAL_SHOW(t.host(), String("localHost"));
	CHECK_EQUAL_SHOW(t.port(), 43210);
	CHECK_EQUAL_SHOW(t.path(), String("/partOne/PartTwo/partThree"));

	CHECK_IS_TRUE(t.set(String("https://host/path")));
	CHECK_IS_TRUE(t.secure());
	CHECK_EQUAL_SHOW(t.port(), 443);

	CHECK_IS_TRUE(t.set(String("http://host/path")));
	CHECK_IS_FALSE(t.secure());
	CHECK_EQUAL_SHOW(t.port(), 80);

	CHECK_IS_FALSE(t.set(String("httpp://host/path")));
	CHECK_IS_FALSE(t.set(String("http//host/path")));
	CHECK_IS_FALSE(t.set(String("http:/host/path")));
	CHECK_IS_FALSE(t.set(String("http://host")));

	CHECK_IS_TRUE(t.set(String("http://host:/path")));
	CHECK_EQUAL_SHOW(t.port(), 80);

	String c = "http://host:1234/path";
	CHECK_IS_TRUE(t.set(c));
	CHECK_EQUAL_SHOW(t.get(), c);

	c = "https://host:1/path/other";
	CHECK_IS_TRUE(t.set(c));
	CHECK_EQUAL_SHOW(t.get(), c);

	c = "http://host/path";
	CHECK_IS_TRUE(t.set(c));
	CHECK_EQUAL_SHOW(t.get(), c);

	c = "https://host/path/other";
	CHECK_IS_TRUE(t.set(c));
	CHECK_EQUAL_SHOW(t.get(), c);
}

#endif
