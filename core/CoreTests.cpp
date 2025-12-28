#define BOOST_TEST_MODULE CoreTestSuite
#include <boost/test/included/unit_test.hpp>

#include "RoarConfig.h"

BOOST_AUTO_TEST_CASE(Lexer) {
    RoarLexer lexer;
    const char *str = "component foo start end\n";

    lexer.fromString(str);

    BOOST_CHECK(lexer.content == "component foo start end\n");
    BOOST_CHECK(lexer.nextToken() == "component");
    BOOST_CHECK(lexer.nextToken() == "foo");
}
