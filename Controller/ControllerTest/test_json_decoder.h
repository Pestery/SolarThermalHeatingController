#ifndef TEST_JSON_DECODER_H
#define TEST_JSON_DECODER_H

// Include headers
#include "../Controller/json_decoder.h"
#include "unit_test.h"

TEST(JsonDecoder) {

	String source = "{  \"name:1\" :  123 , \"name,\\\"2\" : \"a{b}c\" ,\"name:3\": {\"hello\":456,\"world\":789}}";

	JsonDecoder a(source);

	CHECK_IS_TRUE(a.fetch());
	CHECK_EQUAL_SHOW(a.name(), "\"name:1\"");
	CHECK_EQUAL_SHOW(a.value(), "123");

	CHECK_IS_TRUE(a.fetch());
	CHECK_EQUAL_SHOW(a.name(), "\"name,\\\"2\"");
	CHECK_EQUAL_SHOW(a.value(), "\"a{b}c\"");

	CHECK_IS_TRUE(a.fetch());
	CHECK_EQUAL_SHOW(a.name(), "\"name:3\"");
	CHECK_EQUAL_SHOW(a.value(), "{\"hello\":456,\"world\":789}");

	source = a.value();
	CHECK_IS_FALSE(a.fetch());

	JsonDecoder b(source);

	CHECK_IS_TRUE(b.fetch());
	CHECK_EQUAL_SHOW(b.name(), "\"hello\"");
	CHECK_EQUAL_SHOW(b.value(), "456");

	CHECK_IS_TRUE(b.fetch());
	CHECK_EQUAL_SHOW(b.name(), "\"world\"");
	CHECK_EQUAL_SHOW(b.value(), "789");

	CHECK_IS_FALSE(b.fetch());


	source = "    ";
	JsonDecoder c(source);
	CHECK_IS_FALSE(c.fetch());

	JsonDecoder d(nullptr);
	CHECK_IS_FALSE(d.fetch());

}

#endif
