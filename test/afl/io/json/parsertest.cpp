/**
  *  \file test/afl/io/json/parsertest.cpp
  *  \brief Test for afl::io::json::Parser
  */

#include "afl/io/json/parser.hpp"

#include "afl/data/booleanvalue.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/io/bufferedstream.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

using afl::data::BooleanValue;
using afl::data::DefaultValueFactory;
using afl::data::FloatValue;
using afl::data::Hash;
using afl::data::HashValue;
using afl::data::IntegerValue;
using afl::data::StringValue;
using afl::data::Vector;
using afl::data::VectorValue;
using afl::except::FileFormatException;
using afl::except::FileTooShortException;
using afl::io::BufferedStream;
using afl::io::ConstMemoryStream;
using afl::io::json::Parser;
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

/*
 *  Keyword values
 */

AFL_TEST("afl.io.json.Parser:true", a) {
    std::auto_ptr<afl::data::Value> result(parseString(" true"));
    a.checkNonNull("must be BooleanValue", dynamic_cast<BooleanValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<BooleanValue*>(result.get())->getValue(), 1);
}

AFL_TEST("afl.io.json.Parser:false", a) {
    std::auto_ptr<afl::data::Value> result(parseString("false"));
    a.checkNonNull("must be BooleanValue", dynamic_cast<BooleanValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<BooleanValue*>(result.get())->getValue(), 0);
}

AFL_TEST("afl.io.json.Parser:null", a) {
    a.checkNull("plain", parseString("null"));
    a.checkNull("whitespace", parseString("           \n         null"));
}

AFL_TEST("afl.io.json.Parser:invalid-keyword", a) {
    AFL_CHECK_THROWS(a("1"), parseString("null1"), FileFormatException);
    AFL_CHECK_THROWS(a("2"), parseString("null."), FileFormatException);
    AFL_CHECK_THROWS(a("3"), parseString("nullx"), FileFormatException);
    AFL_CHECK_THROWS(a("4"), parseString("nul"),   FileFormatException);
    AFL_CHECK_THROWS(a("5"), parseString("nul_"),  FileFormatException);
    AFL_CHECK_THROWS(a("6"), parseString("Null"),  FileFormatException);
    AFL_CHECK_THROWS(a("7"), parseString("null}"), FileFormatException);
}

/*
 *  Strings
 */

AFL_TEST("afl.io.json.Parser:string:empty", a) {
    std::auto_ptr<afl::data::Value> result(parseString("\"\""));
    a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(), "");
}

AFL_TEST("afl.io.json.Parser:string:nonempty", a) {
    std::auto_ptr<afl::data::Value> result(parseString("  \"foo\""));
    a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(), "foo");
}

AFL_TEST("afl.io.json.Parser:string:escape", a) {
    std::auto_ptr<afl::data::Value> result(parseString("  \"\\n\\t\\r\\f\\b\\\\\\\"x\""));
    a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(), "\n\t\r\f\b\\\"x");
}

AFL_TEST("afl.io.json.Parser:string:sq", a) {
    // Nonstandard!
    std::auto_ptr<afl::data::Value> result(parseString("  \"\\'x\\'\""));
    a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(), "'x'");
}

AFL_TEST("afl.io.json.Parser:string:slash", a) {
    std::auto_ptr<afl::data::Value> result(parseString("  \"<\\/script>\""));
    a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(), "</script>");
}

AFL_TEST("afl.io.json.Parser:string:unicode", a) {
    // Avoid that the C++ compiler sees a Unicode escape here
    std::auto_ptr<afl::data::Value> result(parseString("\"\\""u1234\""));
    a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(), "\xE1\x88\xB4");
}

AFL_TEST("afl.io.json.Parser:string:unicode-mixed", a) {
    // Avoid that the C++ compiler sees a Unicode escape here
    std::auto_ptr<afl::data::Value> result(parseString("\"\\""uCaFe\""));
    a.checkNonNull("must be StringValue", dynamic_cast<StringValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<StringValue*>(result.get())->getValue(), "\xEC\xAB\xBE");
}

AFL_TEST("afl.io.json.Parser:string:invalid", a) {
    AFL_CHECK_THROWS(a("1"), parseString("\"xyz"),       FileFormatException);
    AFL_CHECK_THROWS(a("2"), parseString("'a'"),         FileFormatException);
    AFL_CHECK_THROWS(a("3"), parseString("\"\\""u123"),  FileFormatException);
    AFL_CHECK_THROWS(a("4"), parseString("\"\\""U9999"), FileFormatException);
    AFL_CHECK_THROWS(a("5"), parseString("\"\"xyz"),     FileFormatException);
    AFL_CHECK_THROWS(a("6"), parseString("\"a\nb\""),    FileFormatException);
}

/*
 *  Integers
 */

AFL_TEST("afl.io.json.Parser:int:0", a) {
    std::auto_ptr<afl::data::Value> result(parseString("0"));
    a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 0);
}

AFL_TEST("afl.io.json.Parser:int:pos", a) {
    std::auto_ptr<afl::data::Value> result(parseString("48"));
    a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 48);
}

AFL_TEST("afl.io.json.Parser:int:max", a) {
    std::auto_ptr<afl::data::Value> result(parseString("2147483647"));
    a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), 2147483647);
}

AFL_TEST("afl.io.json.Parser:int:neg", a) {
    std::auto_ptr<afl::data::Value> result(parseString("-99"));
    a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), -99);
}

AFL_TEST("afl.io.json.Parser:int:min", a) {
    std::auto_ptr<afl::data::Value> result(parseString("-2147483648"));
    a.checkNonNull("must be IntegerValue", dynamic_cast<IntegerValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<IntegerValue*>(result.get())->getValue(), -2147483647-1);
}

AFL_TEST("afl.io.json.Parser:int:invalid", a) {
    AFL_CHECK_THROWS(a("hex"),    parseString("0x1"), FileFormatException);
    AFL_CHECK_THROWS(a("signed"), parseString("+1"),  FileFormatException);
    // Invalid JSON, but we accept it anyway:
    // AFL_CHECK_THROWS(a("padded"), parseString("0001"), FileFormatException);
}

/*
 *  Float
 */

AFL_TEST("afl.io.json.Parser:float:zero", a) {
    std::auto_ptr<afl::data::Value> result(parseString("0.0"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), 0.0);
}

AFL_TEST("afl.io.json.Parser:float:pos", a) {
    std::auto_ptr<afl::data::Value> result(parseString("4.75"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), 4.75);
}

AFL_TEST("afl.io.json.Parser:float:maxint", a) {
    std::auto_ptr<afl::data::Value> result(parseString("2147483648"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), 2147483648.0);
}

AFL_TEST("afl.io.json.Parser:float:neg", a) {
    std::auto_ptr<afl::data::Value> result(parseString("-99.5"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), -99.5);
}

AFL_TEST("afl.io.json.Parser:float:minint", a) {
    std::auto_ptr<afl::data::Value> result(parseString("-2147483649"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), -2147483649.0);
}

AFL_TEST("afl.io.json.Parser:float:exp", a) {
    std::auto_ptr<afl::data::Value> result(parseString("1e3"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), 1000.0);
}

AFL_TEST("afl.io.json.Parser:float:exp-fract", a) {
    std::auto_ptr<afl::data::Value> result(parseString("1.5e+1"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), 15.0);
}

AFL_TEST("afl.io.json.Parser:float:neg-exp", a) {
    std::auto_ptr<afl::data::Value> result(parseString("125e-3"));
    a.checkNonNull("must be FloatValue", dynamic_cast<FloatValue*>(result.get()));
    a.checkEqual("check value", dynamic_cast<FloatValue*>(result.get())->getValue(), 0.125);
}

AFL_TEST("afl.io.json.Parser:float:invalid", a) {
    AFL_CHECK_THROWS(a("sign"),    parseString("+1.0"), FileFormatException);
    AFL_CHECK_THROWS(a("formula"), parseString("1+1"),  FileFormatException);
}

/*
 *  Hash
 */

AFL_TEST("afl.io.json.Parser:hash:empty", a) {
    std::auto_ptr<afl::data::Value> result(parseString("{}"));
    Hash& hash = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    a.checkEqual("getNumNames", hash.getKeys().getNumNames(), 0U);
}

AFL_TEST("afl.io.json.Parser:hash:simple", a) {
    std::auto_ptr<afl::data::Value> result(parseString("{\"a\":1,\"b\":2}"));
    Hash& hash      = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    IntegerValue& va = a.checkNonNull("key a type", dynamic_cast<IntegerValue*>(hash.get("a")));
    IntegerValue& vb = a.checkNonNull("key b type", dynamic_cast<IntegerValue*>(hash.get("b")));
    a.checkEqual("value a", va.getValue(), 1);
    a.checkEqual("value b", vb.getValue(), 2);
}

AFL_TEST("afl.io.json.Parser:hash:simple-whitespace", a) {
    std::auto_ptr<afl::data::Value> result(parseString(" { \"a\" : 1 , \"b\" : 2 } "));
    Hash& hash = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    IntegerValue& va = a.checkNonNull("key a type", dynamic_cast<IntegerValue*>(hash.get("a")));
    IntegerValue& vb = a.checkNonNull("key b type", dynamic_cast<IntegerValue*>(hash.get("b")));
    a.checkEqual("value a", va.getValue(), 1);
    a.checkEqual("value b", vb.getValue(), 2);
}

AFL_TEST("afl.io.json.Parser:hash:nested", a) {
    std::auto_ptr<afl::data::Value> result(parseString("{\"a\":{},\"b\":[]}"));
    Hash& hash = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    a.checkNonNull("key a type", dynamic_cast<HashValue*>(hash.get("a")));
    a.checkNonNull("key b type", dynamic_cast<VectorValue*>(hash.get("b")));
}

// Complex examples (actual responses of PCc API)
// (just check that they parse)
AFL_TEST("afl.io.json.Parser:hash:complex", a) {
    std::auto_ptr<afl::data::Value> result;
    AFL_CHECK_SUCCEEDS(a("first"), result.reset(parseString("{\"requests\":{\"mprofile\":{\"allowGet\":1,\"info\":\"Get multiple user's profiles\",\"optionalArgs\":\n"
                                                            "[],\"requiredArgs\":[{\"name\":\"users\",\"type\":\"string_array\"}]},\"profile\":{\"allowGet\":\n"
                                                            "1,\"info\":\"Get one user's profile\",\"optionalArgs\":[],\"requiredArgs\":[{\"name\":\"user\",\n"
                                                            "\"type\":\"string\"}]},\"ranking\":{\"allowGet\":1,\"info\":\"Get user ranking\",\"optionalArgs\":\n"
                                                            "[{\"name\":\"sort\",\"type\":\"string\"},{\"defaultValue\":\"name\",\"name\":\"fields\",\"type\":\"string_array\"}\n"
                                                            ",{\"defaultValue\":0,\"name\":\"reverse\",\"type\":\"int\"}],\"requiredArgs\":[]},\"whoami\":{\"allowGet\":\n"
                                                            "1,\"info\":\"Get information about current user\",\"optionalArgs\":[],\"requiredArgs\":[]\n"
                                                            "}},\"result\":1}")));
    AFL_CHECK_SUCCEEDS(a("second"), result.reset(parseString("{\"result\":1,\"ufids\":[1,2,100,102,105,106,107]}")));
}

AFL_TEST("afl.io.json.Parser:hash:error", a) {
    AFL_CHECK_THROWS(a("1"), parseString("{"), FileFormatException);
    AFL_CHECK_THROWS(a("2"), parseString("{1:2}"), FileFormatException);
    AFL_CHECK_THROWS(a("3"), parseString("{a:\"b\"}"), FileFormatException);
    AFL_CHECK_THROWS(a("4"), parseString("{\"a\",\"b\"}"), FileFormatException);
    AFL_CHECK_THROWS(a("5"), parseString("{\"a\":1"), FileFormatException);
    AFL_CHECK_THROWS(a("6"), parseString("{\"a\":1,}"), FileFormatException);
    AFL_CHECK_THROWS(a("7"), parseString("{\"a\":1,,\"b\":2}"), FileFormatException);
}

/*
 *  Vector
 */

AFL_TEST("afl.io.json.Parser:vector:empty", a) {
    std::auto_ptr<afl::data::Value> result(parseString("[]"));
    Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
    a.checkEqual("check size", vec.size(), 0U);
}

AFL_TEST("afl.io.json.Parser:vector:simple", a) {
    std::auto_ptr<afl::data::Value> result(parseString("[1,2,3]"));
    Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
    a.checkEqual("", vec.size(), 3U);
    a.checkEqual("value 0", a.checkNonNull("type 0", dynamic_cast<IntegerValue*>(vec.get(0))).getValue(), 1);
    a.checkEqual("value 1", a.checkNonNull("type 1", dynamic_cast<IntegerValue*>(vec.get(1))).getValue(), 2);
    a.checkEqual("value 2", a.checkNonNull("type 2", dynamic_cast<IntegerValue*>(vec.get(2))).getValue(), 3);
}

AFL_TEST("afl.io.json.Parser:vector:simple-whitespace", a) {
    // Same, with whitespace
    std::auto_ptr<afl::data::Value> result(parseString(" [ 1 , 2 , 3 ] "));
    Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
    a.checkEqual("", vec.size(), 3U);
    a.checkEqual("value 0", a.checkNonNull("type 0", dynamic_cast<IntegerValue*>(vec.get(0))).getValue(), 1);
    a.checkEqual("value 1", a.checkNonNull("type 1", dynamic_cast<IntegerValue*>(vec.get(1))).getValue(), 2);
    a.checkEqual("value 2", a.checkNonNull("type 2", dynamic_cast<IntegerValue*>(vec.get(2))).getValue(), 3);
}

AFL_TEST("afl.io.json.Parser:vector:nested", a) {
    std::auto_ptr<afl::data::Value> result(parseString("[{},[]]"));
    Vector& vec = *a.checkNonNull("must be VectorValue", dynamic_cast<VectorValue*>(result.get())).getValue();
    a.checkEqual("", vec.size(), 2U);
    a.checkNonNull("index 0",  dynamic_cast<HashValue*>(vec.get(0)));
    a.checkNonNull("index 1", dynamic_cast<VectorValue*>(vec.get(1)));
}

AFL_TEST("afl.io.json.Parser:vector:error", a) {
    AFL_CHECK_THROWS(a("1"), parseString("["), FileFormatException);
    AFL_CHECK_THROWS(a("2"), parseString("[1,]"), FileFormatException);
    AFL_CHECK_THROWS(a("3"), parseString("[1,2"), FileFormatException);
    AFL_CHECK_THROWS(a("4"), parseString("[1,,2]"), FileFormatException);
    AFL_CHECK_THROWS(a("5"), parseString("[1,,2["), FileFormatException);
}

/*
 *  Errors (coverage)
 */

AFL_TEST("afl.io.json.Parser:errors", a) {
    // Nothing in buffer
    AFL_CHECK_THROWS(a("empty1"), parseString(""), FileTooShortException);
    AFL_CHECK_THROWS(a("empty2"), parseString("       "), FileTooShortException);
    AFL_CHECK_THROWS(a("empty3"), parseString("  \r\n\t     "), FileTooShortException);

    // Invalid literal
    AFL_CHECK_THROWS(a("lit1"), parseString("TRUE"), FileFormatException);
    AFL_CHECK_THROWS(a("lit2"), parseString("&x"), FileFormatException);
    AFL_CHECK_THROWS(a("lit3"), parseString("<foo>"), FileFormatException);
    AFL_CHECK_THROWS(a("lit4"), parseString("'a'"), FileFormatException);

    // EOF while expecting single character
    AFL_CHECK_THROWS(a("eof1"), parseString("{"), FileTooShortException);
    AFL_CHECK_THROWS(a("eof2"), parseString("{\"foo\""), FileTooShortException);
    AFL_CHECK_THROWS(a("eof3"), parseString("{\"foo\":1,"), FileTooShortException);

    // Expecting different characters
    AFL_CHECK_THROWS(a("eof4"), parseString("[1"), FileTooShortException);
    AFL_CHECK_THROWS(a("eof5"), parseString("[1:"), FileFormatException);
    AFL_CHECK_THROWS(a("eof6"), parseString("{\"foo\":2"), FileTooShortException);
    AFL_CHECK_THROWS(a("eof7"), parseString("{\"foo\":2:"), FileFormatException);

    // Bad unicode escape
    AFL_CHECK_THROWS(a("uni1"), parseString("\"\\ufooo"), FileFormatException);
    AFL_CHECK_THROWS(a("uni2"), parseString("\"\\uf"), FileTooShortException);
    AFL_CHECK_THROWS(a("uni3"), parseString("\"\\u0x10"), FileFormatException);
}

/*
 *  Duplicate keys in a hash.
 *  Must reliably pick the last value.
 */

// Regular case
AFL_TEST("afl.io.json.Parser:hash:dup-normal", a) {
    std::auto_ptr<afl::data::Value> result(parseString("{\"qa\":1,\"qa\":2}"));
    Hash& hash = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    a.checkEqual("num values", hash.getValues().size(), 1U);
    a.checkEqual("num names", hash.getKeys().getNumNames(), 1U);
    a.checkEqual("check value", a.checkNonNull("check type", dynamic_cast<IntegerValue*>(hash.get("qa"))).getValue(), 2);
}

// Replace, not consecutive
AFL_TEST("afl.io.json.Parser:hash:dup-not-consecutive", a) {
    std::auto_ptr<afl::data::Value> result(parseString("{\"qa\":[1,\"x\"],\"qb\":99, \"qa\":2}"));
    Hash& hash = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    a.checkEqual("num values", hash.getValues().size(), 2U);
    a.checkEqual("num names", hash.getKeys().getNumNames(), 2U);
    a.checkEqual("check value qa", a.checkNonNull("check type qa", dynamic_cast<IntegerValue*>(hash.get("qa"))).getValue(), 2);
    a.checkEqual("check value qb", a.checkNonNull("check type qb", dynamic_cast<IntegerValue*>(hash.get("qb"))).getValue(), 99);
}

// Replace by different type case
AFL_TEST("afl.io.json.Parser:hash:dup-different-type", a) {
    std::auto_ptr<afl::data::Value> result(parseString("{\"qa\":[1,\"x\"],\"qa\":2}"));
    Hash& hash = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    a.checkEqual("num values", hash.getValues().size(), 1U);
    a.checkEqual("num names", hash.getKeys().getNumNames(), 1U);
    a.checkEqual("check value", a.checkNonNull("check type", dynamic_cast<IntegerValue*>(hash.get("qa"))).getValue(), 2);
}

// Replace by null
AFL_TEST("afl.io.json.Parser:hash:dup-null", a) {
    std::auto_ptr<afl::data::Value> result(parseString("{\"qa\":\"x\",\"qa\":null}"));
    Hash& hash = *a.checkNonNull("must be HashValue", dynamic_cast<HashValue*>(result.get())).getValue();
    a.checkEqual("num names", hash.getKeys().getNumNames(), 1U);
    a.checkNull("check value", hash.get("qa"));
}
