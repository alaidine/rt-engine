#define BOOST_TEST_MODULE CoreTestSuite
#include <boost/test/included/unit_test.hpp>

#include "RTEConfig.h"

BOOST_AUTO_TEST_CASE(Lexer)
{
    RTELexer lexer;
    const char* str = "type scene\n";

    lexer.fromString(str);

    BOOST_CHECK(lexer.content == "type scene\n");
    BOOST_CHECK(lexer.nextToken() == "type");
    BOOST_CHECK(lexer.nextToken() == "scene");
    BOOST_CHECK(lexer.nextToken() == "\n");
    BOOST_CHECK(lexer.nextToken() == "EOF");
}
