/**
  *  \file u/t_io_resp_parser.cpp
  *  \brief Test for afl::io::resp::Parser
  */

#include "afl/io/resp/parser.hpp"

#include <memory>
#include "u/t_io_resp.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/string/string.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/data/errorvalue.hpp"

using afl::io::resp::Parser;
using afl::data::DefaultValueFactory;
using afl::string::toBytes;

namespace {
    /* Parse string using afl::io::resp::Parser. */
    afl::data::Value* parseString(const char* str)
    {
        DefaultValueFactory factory;
        afl::io::resp::Parser parser(factory);
        afl::base::ConstBytes_t bytes(toBytes(str));

        bool result = parser.handleData("<source name>", bytes);
        TSM_ASSERT(str, result);
        TSM_ASSERT(str, bytes.empty());

        return parser.extract();
    }

    /* Parse incomplete string. */
    void parseIncomplete(const char* str)
    {
        DefaultValueFactory factory;
        afl::io::resp::Parser parser(factory);
        afl::base::ConstBytes_t bytes(toBytes(str));

        bool result = parser.handleData("<source name>", bytes);
        TSM_ASSERT(str, !result);
        TSM_ASSERT(str, bytes.empty());
    }
}

/** Test integer parsing. */
void
TestIoRespParser::testIntegers()
{
    // Successes
    {
        std::auto_ptr<afl::data::Value> result(parseString(":0\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString(":000000000000000000000000000000000000000000000000000\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString(":12345\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 12345);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString(":-98765\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == -98765);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString(":2147483647\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 2147483647);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString(":+2147483647\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 2147483647);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString(":-2147483648\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == -2147483647-1);
    }

    // Errors
    parseIncomplete(":");
    parseIncomplete(":1");
    TS_ASSERT_THROWS(parseString("::"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":++1\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":-+1\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":+-1\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":--1\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":2147483648\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":+2147483648\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":-2147483649\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":-3333333333\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":+3333333333\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":3333333333\r\n"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString(":0x123\r\n"), afl::except::FileFormatException);
}

/** Test string (bulk) parsing. */
void
TestIoRespParser::testString()
{
    // Null
    TS_ASSERT(parseString("$-1\r\n") == 0);

    // Some strings
    {
        std::auto_ptr<afl::data::Value> result(parseString("$0\r\n\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "");
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("$2\r\n\r\n\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "\r\n");
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("$00000000000002\r\n\r\n\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "\r\n");
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("$3\r\nabc\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "abc");
    }

    // Long string
    {
        String_t longString(9999, 'x');
        std::auto_ptr<afl::data::Value> result(parseString(("$9999\r\n" + longString + "\r\n").c_str()));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == longString);
    }

    // Errors
    parseIncomplete("$");
    parseIncomplete("$0");
    parseIncomplete("$0\r\n");
    parseIncomplete("$1\r\n");
    parseIncomplete("$1\r\na");
    TS_ASSERT_THROWS(parseString("$1\r\nxx\r\n"), afl::except::FileFormatException);
}

/** Test vector (multi-bulk) parsing. */
void
TestIoRespParser::testVector()
{
    // Null
    TS_ASSERT(parseString("*-1\r\n") == 0);

    // Empty
    {
        std::auto_ptr<afl::data::Value> result(parseString("*0\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ptr<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 0U);
    }

    // Simple
    {
        std::auto_ptr<afl::data::Value> result(parseString("*3\r\n:1\r\n:2\r\n:3\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ptr<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 3U);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(0)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(0))->getValue(), 1);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(1)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(1))->getValue(), 2);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(2)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(2))->getValue(), 3);
    }

    // Totally mixed
    {
        std::auto_ptr<afl::data::Value> result(parseString("*4\r\n"
                                                           ":1\r\n"
                                                           "$3\r\n"
                                                           "huh\r\n"
                                                           "$-1\r\n"
                                                           "+OK\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ptr<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 4U);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(0)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(0))->getValue(), 1);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(vec->get(1)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::StringValue*>(vec->get(1))->getValue(), "huh");
        TS_ASSERT(vec->get(2) == 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(vec->get(3)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::StringValue*>(vec->get(3))->getValue(), "OK");
    }

    // Errors
    parseIncomplete("*");
    parseIncomplete("*0");
    parseIncomplete("*1\r\n");
    TS_ASSERT_THROWS(parseString("*1\r\na"), afl::except::FileFormatException);
}

/** Test one-liners (ok/error). */
void
TestIoRespParser::testOneliners()
{
    // Success messages
    {
        std::auto_ptr<afl::data::Value> result(parseString("+\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "");
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("+OK\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "OK");
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("++123\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "+123");
    }

    // Failure messages
    {
        std::auto_ptr<afl::data::Value> result(parseString("-fail\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::ErrorValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::ErrorValue*>(result.get())->getText() == "fail");
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("-\r\n"));
        TS_ASSERT(dynamic_cast<afl::data::ErrorValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::ErrorValue*>(result.get())->getText() == "");
    }

    // Errors
    parseIncomplete("+");
    parseIncomplete("+ok");
    parseIncomplete("-");
    parseIncomplete("-fail");
}

/** Test short form. */
void
TestIoRespParser::testShortForm()
{
    DefaultValueFactory factory;

    // Default is not accepted
    TS_ASSERT_THROWS(parseString("foo bar\r\n"), afl::except::FileFormatException);

    // Success
    {
        afl::base::ConstBytes_t bytes(toBytes("foo bar\r\n"));
        Parser p(factory);

        p.setAcceptShortForm(true);
        TS_ASSERT(p.handleData("<source name>", bytes));

        std::auto_ptr<afl::data::Value> result(p.extract());
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ptr<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 2U);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(vec->get(0)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::StringValue*>(vec->get(0))->getValue(), "foo");
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(vec->get(1)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::StringValue*>(vec->get(1))->getValue(), "bar");
    }

    // Error
    {
        afl::base::ConstBytes_t bytes(toBytes("foo bar"));
        Parser p(factory);
        p.setAcceptShortForm(true);
        TS_ASSERT(!p.handleData("<source name>", bytes));
    }
}
