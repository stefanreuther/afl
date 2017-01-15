/**
  *  \file u/t_io_json_parser.cpp
  *  \brief Test for afl::io::json::Parser
  */

#include <memory>
#include "afl/io/json/parser.hpp"

#include "u/t_io_json.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/io/bufferedstream.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/string/string.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/except/filetooshortexception.hpp"

using afl::io::json::Parser;
using afl::data::DefaultValueFactory;
using afl::io::BufferedStream;
using afl::io::ConstMemoryStream;
using afl::string::toBytes;

namespace {
    afl::data::Value* parseString(const char* str)
    {
        DefaultValueFactory factory;
        ConstMemoryStream cms(toBytes(str));
        BufferedStream bs(cms);
        return Parser(bs, factory).parseComplete();
    }
}

/** Test keyword values. */
void
TestIoJsonParser::testKeywords()
{
    // true
    {
        std::auto_ptr<afl::data::Value> result(parseString(" true"));
        TS_ASSERT(dynamic_cast<afl::data::BooleanValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::BooleanValue*>(result.get())->getValue() == 1);
    }
    // false
    {
        std::auto_ptr<afl::data::Value> result(parseString("false"));
        TS_ASSERT(dynamic_cast<afl::data::BooleanValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::BooleanValue*>(result.get())->getValue() == 0);
    }
    // null
    TS_ASSERT(parseString("null") == 0);
    TS_ASSERT(parseString("           \n         null") == 0);

    // invalid
    TS_ASSERT_THROWS(parseString("null1"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("null."), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("nullx"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("nul"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("nul_"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("Null"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("null}"), afl::except::FileFormatException);
}

/** Test strings. */
void
TestIoJsonParser::testString()
{
    // Empty
    {
        std::auto_ptr<afl::data::Value> result(parseString("\"\""));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "");
    }
    // Nonempty
    {
        std::auto_ptr<afl::data::Value> result(parseString("  \"foo\""));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "foo");
    }
    // Escape
    {
        std::auto_ptr<afl::data::Value> result(parseString("  \"\\n\\t\\r\\f\\b\\\\\\\"x\""));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "\n\t\r\f\b\\\"x");
    }
    // Single-quote escape (nonstandard)
    {
        std::auto_ptr<afl::data::Value> result(parseString("  \"\\'x\\'\""));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "'x'");
    }
    // Slash
    {
        std::auto_ptr<afl::data::Value> result(parseString("  \"<\\/script>\""));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "</script>");
    }
    // Unicode
    {
        // Avoid that the C++ compiler sees a Unicode escape here
        std::auto_ptr<afl::data::Value> result(parseString("\"\\""u1234\""));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "\xE1\x88\xB4");
    }
    // Unicode
    {
        // Avoid that the C++ compiler sees a Unicode escape here
        std::auto_ptr<afl::data::Value> result(parseString("\"\\""uCaFe\""));
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::StringValue*>(result.get())->getValue() == "\xEC\xAB\xBE");
    }
    // Invalid
    TS_ASSERT_THROWS(parseString("\"xyz"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("'a'"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("\"\\""u123"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("\"\\""U9999"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("\"\"xyz"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("\"a\nb\""), afl::except::FileFormatException);
}

/** Test integers. */
void
TestIoJsonParser::testInteger()
{
    // Some numbers
    {
        std::auto_ptr<afl::data::Value> result(parseString("0"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("48"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 48);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("2147483647"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == 2147483647);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("-99"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == -99);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("-2147483648"));
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue() == -2147483647-1);
    }

    // Invalid
    TS_ASSERT_THROWS(parseString("0x1"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("+1"), afl::except::FileFormatException);
    // Invalid JSON, but we accept it anyway:
    // TS_ASSERT_THROWS(parseString("0001"), afl::except::FileFormatException);
}

/** Test floating-point numbers. */
void
TestIoJsonParser::testFloat()
{
    // Some numbers
    {
        std::auto_ptr<afl::data::Value> result(parseString("0.0"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == 0.0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("4.75"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == 4.75);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("2147483648"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == 2147483648.0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("-99.5"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == -99.5);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("-2147483649"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == -2147483649.0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("1e3"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == 1000.0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("1.5e+1"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == 15.0);
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("125e-3"));
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get()) != 0);
        TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(result.get())->getValue() == 0.125);
    }

    // Invalid
    TS_ASSERT_THROWS(parseString("+1.0"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("1+1"), afl::except::FileFormatException);
}

/** Test hashes. */
void
TestIoJsonParser::testHash()
{
    // Empty
    {
        std::auto_ptr<afl::data::Value> result(parseString("{}"));
        TS_ASSERT(dynamic_cast<afl::data::HashValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Hash> hash(dynamic_cast<afl::data::HashValue*>(result.get())->getValue());
        TS_ASSERT(&hash.get() != 0);
        TS_ASSERT_EQUALS(hash->getKeys().getNumNames(), 0U);
    }

    // Simple
    {
        std::auto_ptr<afl::data::Value> result(parseString("{\"a\":1,\"b\":2}"));
        TS_ASSERT(dynamic_cast<afl::data::HashValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Hash> hash(dynamic_cast<afl::data::HashValue*>(result.get())->getValue());
        TS_ASSERT(&hash.get() != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(hash->get("a")) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(hash->get("a"))->getValue(), 1);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(hash->get("b")) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(hash->get("b"))->getValue(), 2);
    }

    // Same, with whitespace
    {
        std::auto_ptr<afl::data::Value> result(parseString(" { \"a\" : 1 , \"b\" : 2 } "));
        TS_ASSERT(dynamic_cast<afl::data::HashValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Hash> hash(dynamic_cast<afl::data::HashValue*>(result.get())->getValue());
        TS_ASSERT(&hash.get() != 0);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(hash->get("a")) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(hash->get("a"))->getValue(), 1);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(hash->get("b")) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(hash->get("b"))->getValue(), 2);
    }

    // Nested
    {
        std::auto_ptr<afl::data::Value> result(parseString("{\"a\":{},\"b\":[]}"));
        TS_ASSERT(dynamic_cast<afl::data::HashValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Hash> hash(dynamic_cast<afl::data::HashValue*>(result.get())->getValue());
        TS_ASSERT(&hash.get() != 0);
        TS_ASSERT(dynamic_cast<afl::data::HashValue*>(hash->get("a")) != 0);
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(hash->get("b")) != 0);
    }

    // Complex examples (actual responses of PCc API)
    // (just check that they parse)
    {
        std::auto_ptr<afl::data::Value> result(parseString("{\"requests\":{\"mprofile\":{\"allowGet\":1,\"info\":\"Get multiple user's profiles\",\"optionalArgs\":\n"
                                                           "[],\"requiredArgs\":[{\"name\":\"users\",\"type\":\"string_array\"}]},\"profile\":{\"allowGet\":\n"
                                                           "1,\"info\":\"Get one user's profile\",\"optionalArgs\":[],\"requiredArgs\":[{\"name\":\"user\",\n"
                                                           "\"type\":\"string\"}]},\"ranking\":{\"allowGet\":1,\"info\":\"Get user ranking\",\"optionalArgs\":\n"
                                                           "[{\"name\":\"sort\",\"type\":\"string\"},{\"defaultValue\":\"name\",\"name\":\"fields\",\"type\":\"string_array\"}\n"
                                                           ",{\"defaultValue\":0,\"name\":\"reverse\",\"type\":\"int\"}],\"requiredArgs\":[]},\"whoami\":{\"allowGet\":\n"
                                                           "1,\"info\":\"Get information about current user\",\"optionalArgs\":[],\"requiredArgs\":[]\n"
                                                           "}},\"result\":1}"));
    }
    {
        std::auto_ptr<afl::data::Value> result(parseString("{\"result\":1,\"ufids\":[1,2,100,102,105,106,107]}"));
    }

    // Errors
    TS_ASSERT_THROWS(parseString("{"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("{1:2}"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("{a:\"b\"}"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("{\"a\",\"b\"}"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("{\"a\":1"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("{\"a\":1,}"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("{\"a\":1,,\"b\":2}"), afl::except::FileFormatException);
}

/** Test vectors. */
void
TestIoJsonParser::testVector()
{
    // Empty
    {
        std::auto_ptr<afl::data::Value> result(parseString("[]"));
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(&vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 0U);
    }

    // Simple
    {
        std::auto_ptr<afl::data::Value> result(parseString("[1,2,3]"));
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(&vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 3U);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(0)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(0))->getValue(), 1);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(1)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(1))->getValue(), 2);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(2)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(2))->getValue(), 3);
    }

    // Same, with whitespace
    {
        std::auto_ptr<afl::data::Value> result(parseString(" [ 1 , 2 , 3 ] "));
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(&vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 3U);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(0)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(0))->getValue(), 1);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(1)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(1))->getValue(), 2);
        TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vec->get(2)) != 0);
        TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(vec->get(2))->getValue(), 3);
    }

    // Nested
    {
        std::auto_ptr<afl::data::Value> result(parseString("[{},[]]"));
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(result.get()) != 0);
        afl::base::Ref<afl::data::Vector> vec(dynamic_cast<afl::data::VectorValue*>(result.get())->getValue());
        TS_ASSERT(&vec.get() != 0);
        TS_ASSERT_EQUALS(vec->size(), 2U);
        TS_ASSERT(dynamic_cast<afl::data::HashValue*>(vec->get(0)) != 0);
        TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(vec->get(1)) != 0);
    }

    // Errors
    TS_ASSERT_THROWS(parseString("["), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("[1,]"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("[1,2"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("[1,,2]"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("[1,,2["), afl::except::FileFormatException);
}

/** Test errors (coverage). */
void
TestIoJsonParser::testErrors()
{
    // Nothing in buffer
    TS_ASSERT_THROWS(parseString(""), afl::except::FileTooShortException);
    TS_ASSERT_THROWS(parseString("       "), afl::except::FileTooShortException);
    TS_ASSERT_THROWS(parseString("  \r\n\t     "), afl::except::FileTooShortException);

    // Invalid literal
    TS_ASSERT_THROWS(parseString("TRUE"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("&x"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("<foo>"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("'a'"), afl::except::FileFormatException);

    // EOF while expecting single character
    TS_ASSERT_THROWS(parseString("{"), afl::except::FileTooShortException);
    TS_ASSERT_THROWS(parseString("{\"foo\""), afl::except::FileTooShortException);
    TS_ASSERT_THROWS(parseString("{\"foo\":1,"), afl::except::FileTooShortException);

    // Expecting different characters
    TS_ASSERT_THROWS(parseString("[1"), afl::except::FileTooShortException);
    TS_ASSERT_THROWS(parseString("[1:"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("{\"foo\":2"), afl::except::FileTooShortException);
    TS_ASSERT_THROWS(parseString("{\"foo\":2:"), afl::except::FileFormatException);

    // Bad unicode escape
    TS_ASSERT_THROWS(parseString("\"\\ufooo"), afl::except::FileFormatException);
    TS_ASSERT_THROWS(parseString("\"\\uf"), afl::except::FileTooShortException);
    TS_ASSERT_THROWS(parseString("\"\\u0x10"), afl::except::FileFormatException);
}
