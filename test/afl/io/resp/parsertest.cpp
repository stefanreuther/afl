/**
  *  \file test/afl/io/resp/parsertest.cpp
  *  \brief Test for afl::io::resp::Parser
  */

#include <memory>

#include "afl/io/resp/parser.hpp"

#include "afl/data/defaultvaluefactory.hpp"
#include "afl/data/errorvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

using afl::data::DefaultValueFactory;
using afl::data::ErrorValue;
using afl::data::IntegerValue;
using afl::data::StringValue;
using afl::data::Value;
using afl::data::Vector;
using afl::data::VectorValue;
using afl::except::FileFormatException;
using afl::io::resp::Parser;
using afl::string::toBytes;

namespace {
    /* Parse string using afl::io::resp::Parser. */
    Value* parseString(const afl::test::Assert& a, const char* str)
    {
        DefaultValueFactory factory;
        Parser parser(factory);
        afl::base::ConstBytes_t bytes(toBytes(str));

        bool result = parser.handleData(bytes);
        a.check("parse success", result);
        a.check("parse complete", bytes.empty());

        return parser.extract();
    }

    /* Parse incomplete string. */
    void parseIncomplete(const afl::test::Assert& a, const char* str)
    {
        DefaultValueFactory factory;
        Parser parser(factory);
        afl::base::ConstBytes_t bytes(toBytes(str));

        bool result = parser.handleData(bytes);
        a.check("parse fails", !result);
        a.check("parse incomplete", bytes.empty());
    }
}

namespace afl { namespace io { namespace resp {

    /*
     *  Integers
     */

    AFL_TEST("afl.io.resp.Parser:int:0", a) {
        std::auto_ptr<Value> result(parseString(a, ":0\r\n"));
        a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 0);
    }

    AFL_TEST("afl.io.resp.Parser:int:long0", a) {
        std::auto_ptr<Value> result(parseString(a, ":000000000000000000000000000000000000000000000000000\r\n"));
        a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 0);
    }

    AFL_TEST("afl.io.resp.Parser:int:pos", a) {
        std::auto_ptr<Value> result(parseString(a, ":12345\r\n"));
        a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 12345);
    }

    AFL_TEST("afl.io.resp.Parser:int:neg", a) {
        std::auto_ptr<Value> result(parseString(a, ":-98765\r\n"));
        a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), -98765);
    }

    AFL_TEST("afl.io.resp.Parser:int:pos-max", a) {
        std::auto_ptr<Value> result(parseString(a, ":2147483647\r\n"));
        a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 2147483647);
    }

    AFL_TEST("afl.io.resp.Parser:int:plus-max", a) {
        std::auto_ptr<Value> result(parseString(a, ":+2147483647\r\n"));
        a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 2147483647);
    }

    AFL_TEST("afl.io.resp.Parser:int:neg-min", a) {
        std::auto_ptr<Value> result(parseString(a, ":-2147483648\r\n"));
        a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), -2147483647-1);
    }

    AFL_TEST("afl.io.resp.Parser:int:incomplete", a) {
        parseIncomplete(a("missing-num"), ":");
        parseIncomplete(a("missing-nl"),  ":1");
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax1", a) {
        AFL_CHECK_THROWS(a, parseString(a, "::"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax2", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax3", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":++1\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax4", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":-+1\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax5", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":+-1\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax6", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":--1\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax7", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":2147483648\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax8", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":+2147483648\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax9", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":-2147483649\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax10", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":-3333333333\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax11", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":+3333333333\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax12", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":3333333333\r\n"), FileFormatException);
    }

    AFL_TEST("afl.io.resp.Parser:int:syntax13", a) {
        AFL_CHECK_THROWS(a, parseString(a, ":0x123\r\n"), FileFormatException);
    }


    /*
     *  Strings
     */

    AFL_TEST("afl.io.resp.Parser:string:null", a) {
        a.checkNull("", parseString(a, "$-1\r\n"));
    }

    AFL_TEST("afl.io.resp.Parser:string:empty", a) {
        std::auto_ptr<Value> result(parseString(a, "$0\r\n\r\n"));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  "");
    }

    AFL_TEST("afl.io.resp.Parser:string:crlf", a)
    {
        std::auto_ptr<Value> result(parseString(a, "$2\r\n\r\n\r\n"));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  "\r\n");
    }

    AFL_TEST("afl.io.resp.Parser:string:long-length", a) {

        std::auto_ptr<Value> result(parseString(a, "$00000000000002\r\n\r\n\r\n"));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  "\r\n");
    }

    AFL_TEST("afl.io.resp.Parser:string:text", a) {
        std::auto_ptr<Value> result(parseString(a, "$3\r\nabc\r\n"));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  "abc");
    }

    // Long string
    AFL_TEST("afl.io.resp.Parser:string:long-string", a) {
        String_t longString(9999, 'x');
        std::auto_ptr<Value> result(parseString(a, ("$9999\r\n" + longString + "\r\n").c_str()));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  longString);
    }

    // Errors
    AFL_TEST("afl.io.resp.Parser:string:incomplete", a) {
        parseIncomplete(a("missing-length"),    "$");
        parseIncomplete(a("missing-first-nl"),  "$0");
        parseIncomplete(a("missing-final-nl"),  "$0\r\n");
        parseIncomplete(a("missing-text"),      "$1\r\n");
        parseIncomplete(a("missing-final-nl2"), "$1\r\na");
    }

    AFL_TEST("afl.io.resp.Parser:string:syntax", a) {
        AFL_CHECK_THROWS(a, parseString(a, "$1\r\nxx\r\n"), FileFormatException);
    }


    /*
     *  Vector (multi-bulk)
     */

    AFL_TEST("afl.io.resp.Parser:vector:null", a) {
        a.checkNull("", parseString(a, "*-1\r\n"));
    }

    // Empty
    AFL_TEST("afl.io.resp.Parser:vector:empty", a) {
        std::auto_ptr<Value> result(parseString(a, "*0\r\n"));
        Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
        a.checkEqual("check size", vec.size(), 0U);
    }

    // Simple
    AFL_TEST("afl.io.resp.Parser:vector:simple", a) {
        std::auto_ptr<Value> result(parseString(a, "*3\r\n:1\r\n:2\r\n:3\r\n"));
        Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
        a.checkEqual("check size", vec.size(), 3U);
        a.checkNonNull("index 0, IntegerValue", dynamic_cast<IntegerValue*>(vec.get(0)));
        a.checkEqual  ("index 0, value",        dynamic_cast<IntegerValue*>(vec.get(0))->getValue(), 1);
        a.checkNonNull("index 1, IntegerValue", dynamic_cast<IntegerValue*>(vec.get(1)));
        a.checkEqual  ("index 1, value",        dynamic_cast<IntegerValue*>(vec.get(1))->getValue(), 2);
        a.checkNonNull("index 2, IntegerValue", dynamic_cast<IntegerValue*>(vec.get(2)));
        a.checkEqual  ("index 2, value",        dynamic_cast<IntegerValue*>(vec.get(2))->getValue(), 3);
    }

    // Totally mixed
    AFL_TEST("afl.io.resp.Parser:vector:mixed", a) {
        std::auto_ptr<Value> result(parseString(a, "*4\r\n"
                                                ":1\r\n"
                                                "$3\r\n"
                                                "huh\r\n"
                                                "$-1\r\n"
                                                "+OK\r\n"));
        Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
        a.checkEqual  ("check size", vec.size(), 4U);
        a.checkNonNull("index 0, IntegerValue", dynamic_cast<IntegerValue*>(vec.get(0)));
        a.checkEqual  ("index 0, value",        dynamic_cast<IntegerValue*>(vec.get(0))->getValue(), 1);
        a.checkNonNull("index 1, StringValue",  dynamic_cast<StringValue*>(vec.get(1)));
        a.checkEqual  ("index 1, value",        dynamic_cast<StringValue*>(vec.get(1))->getValue(), "huh");
        a.checkNull   ("index 2", vec.get(2));
        a.checkNonNull("index 3, StringValue",  dynamic_cast<StringValue*>(vec.get(3)));
        a.checkEqual  ("index 3, value",        dynamic_cast<StringValue*>(vec.get(3))->getValue(), "OK");
    }

    // Errors
    AFL_TEST("afl.io.resp.Parser:vector:incomplete", a) {
        parseIncomplete(a("missing length"),  "*");
        parseIncomplete(a("missing nl"),      "*0");
        parseIncomplete(a("missing content"), "*1\r\n");
    }

    AFL_TEST("afl.io.resp.Parser:vector:error", a) {
        AFL_CHECK_THROWS(a, parseString(a, "*1\r\na"), FileFormatException);
    }

    /*
     *  Oneliners
     */

    // Success
    AFL_TEST("afl.io.resp.Parser:success:empty", a) {
        std::auto_ptr<Value> result(parseString(a, "+\r\n"));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  "");
    }

    AFL_TEST("afl.io.resp.Parser:success:nonempty", a) {
        std::auto_ptr<Value> result(parseString(a, "+OK\r\n"));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  "OK");
    }

    AFL_TEST("afl.io.resp.Parser:success:nonempty2", a) {
        std::auto_ptr<Value> result(parseString(a, "++123\r\n"));
        a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(),  "+123");
    }

    // Failure messages
    AFL_TEST("afl.io.resp.Parser:fail:nonempty", a) {
        std::auto_ptr<Value> result(parseString(a, "-fail\r\n"));
        a.checkNonNull("must be ErrorValue", dynamic_cast<ErrorValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<ErrorValue*>(result.get())->getText(), "fail");
    }

    AFL_TEST("afl.io.resp.Parser:fail:empty", a) {
        std::auto_ptr<Value> result(parseString(a, "-\r\n"));
        a.checkNonNull("must be ErrorValue", dynamic_cast<ErrorValue*>(result.get()));
        a.checkEqual("check value", dynamic_cast<ErrorValue*>(result.get())->getText(), "");
    }

    // Errors
    AFL_TEST("afl.io.resp.Parser:incomplete", a) {
        parseIncomplete(a("success-missing-text"), "+");
        parseIncomplete(a("success-missing-nl"),   "+ok");
        parseIncomplete(a("fail-missing-text"),    "-");
        parseIncomplete(a("fail-missing-nl"),      "-fail");
    }

    /*
     *  Short form
     */

    // Default is not accepted
    AFL_TEST("afl.io.resp.Parser:short:rejected-by-default", a) {
        DefaultValueFactory factory;
        AFL_CHECK_THROWS(a, parseString(a, "foo bar\r\n"), FileFormatException);
    }

    // Success
    AFL_TEST("afl.io.resp.Parser:short:success", a) {
        afl::base::ConstBytes_t bytes(toBytes("foo bar\r\n"));
        DefaultValueFactory factory;
        Parser p(factory);

        p.setAcceptShortForm(true);
        a.check("parse success", p.handleData(bytes));

        std::auto_ptr<Value> result(p.extract());
        Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
        a.checkEqual ("length", vec.size(), 2U);
        a.checkNonNull("index 0, StringValue", dynamic_cast<StringValue*>(vec.get(0)));
        a.checkEqual  ("index 0, value",       dynamic_cast<StringValue*>(vec.get(0))->getValue(), "foo");
        a.checkNonNull("index 1, StringValue", dynamic_cast<StringValue*>(vec.get(1)));
        a.checkEqual  ("index 1, value",       dynamic_cast<StringValue*>(vec.get(1))->getValue(), "bar");
    }

    // Error
    AFL_TEST("afl.io.resp.Parser:short:incomplete", a) {
        afl::base::ConstBytes_t bytes(toBytes("foo bar"));
        DefaultValueFactory factory;
        Parser p(factory);
        p.setAcceptShortForm(true);
        a.check("parse incomplete", !p.handleData(bytes));
    }

} } }
