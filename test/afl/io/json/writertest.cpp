/**
  *  \file test/afl/io/json/writertest.cpp
  *  \brief Test for afl::io::json::Writer
  */

#include "afl/io/json/writer.hpp"

#include "afl/data/booleanvalue.hpp"
#include "afl/data/errorvalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    String_t formatToString(afl::data::Value* p)
    {
        afl::io::InternalSink s;
        afl::io::json::Writer(s).visit(p);
        return afl::string::fromBytes(s.getContent());
    }
}

/*
 *  Simple tests
 */

AFL_TEST("afl.io.json.Writer:simple:null", a) {
    a.checkEqual("", formatToString(0), "null");
}

AFL_TEST("afl.io.json.Writer:simple:true", a) {
    afl::data::BooleanValue bv(true);
    a.checkEqual("", formatToString(&bv), "true");
}

AFL_TEST("afl.io.json.Writer:simple:false", a) {
    afl::data::BooleanValue bv(false);
    a.checkEqual("", formatToString(&bv), "false");
}

AFL_TEST("afl.io.json.Writer:simple:num-pos", a) {
    afl::data::IntegerValue iv(42);
    a.checkEqual("", formatToString(&iv), "42");
}

AFL_TEST("afl.io.json.Writer:simple:num-neg", a) {
    afl::data::IntegerValue iv(-99);
    a.checkEqual("", formatToString(&iv), "-99");
}

AFL_TEST("afl.io.json.Writer:simple:hash", a) {
    afl::base::Ref<afl::data::Hash> hash(afl::data::Hash::create());
    hash->setNew("a", new afl::data::IntegerValue(99));
    hash->setNew("b", new afl::data::BooleanValue(true));
    afl::data::HashValue hv(hash);
    a.checkEqual("", formatToString(&hv), "{\"a\":99,\"b\":true}");
}

AFL_TEST("afl.io.json.Writer:simple:vector", a) {
    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(99));
    vec->pushBackNew(new afl::data::BooleanValue(true));
    vec->pushBackNew(0);
    vec->pushBackNew(new afl::data::StringValue("x"));
    afl::data::VectorValue vv(vec);
    a.checkEqual("", formatToString(&vv), "[99,true,null,\"x\"]");
}

AFL_TEST("afl.io.json.Writer:simple:str-empty", a) {
    afl::data::StringValue sv("");
    a.checkEqual("", formatToString(&sv), "\"\"");
}

AFL_TEST("afl.io.json.Writer:simple:str-normal", a) {
    afl::data::StringValue sv("abc");
    a.checkEqual("", formatToString(&sv), "\"abc\"");
}

AFL_TEST("afl.io.json.Writer:simple:str-complex", a) {
    afl::data::StringValue sv(String_t("\n\t\r\0\f\b\033\\\"a", 10));
    a.checkEqual("", formatToString(&sv), "\"\\n\\t\\r\\""u0000\\f\\b\\""u001B\\\\\\\"a\"");
}

AFL_TEST("afl.io.json.Writer:simple:float-pos", a) {
    afl::data::FloatValue fv(1.0);
    a.checkEqual("", formatToString(&fv), "1");
}

AFL_TEST("afl.io.json.Writer:simple:float-neg", a) {
    afl::data::FloatValue fv(-1.0);
    a.checkEqual("", formatToString(&fv), "-1");
}

AFL_TEST("afl.io.json.Writer:simple:float-frac", a) {
    afl::data::FloatValue fv(-1.5);
    a.checkEqual("", formatToString(&fv), "-1.5");
}

AFL_TEST("afl.io.json.Writer:simple:float-frac2", a) {
    // FIXME: larger values do not produce exact output:
    //   55.25 --> 55.2500000000000000008673617
    //  255.25 --> 255.2500000000000000008673617
    // Effectively, somewhere in the code, the 2**-60 bit gets unexpectedly added.
    afl::data::FloatValue fv(5.25);
    a.checkEqual("", formatToString(&fv), "5.25");
}

AFL_TEST("afl.io.json.Writer:simple:error", a) {
    afl::data::ErrorValue ev("text", "source");
    a.checkEqual("", formatToString(&ev), "null");
}

AFL_TEST("afl.io.json.Writer:simple:other", a) {
    class Other : public afl::data::Value {
     public:
        virtual void visit(afl::data::Visitor& visitor) const
            { visitor.visitOther(*this); }
        virtual Other* clone() const
            { return new Other(); }
    };
    Other ov;
    a.checkEqual("", formatToString(&ov), "null");
}

/*
 *  Formatted output
 */

AFL_TEST("afl.io.json.Writer:format", a) {
    // Some structure
    afl::base::Ref<afl::data::Hash> hash(afl::data::Hash::create());
    hash->setNew("a", new afl::data::IntegerValue(99));
    hash->setNew("b", new afl::data::BooleanValue(true));
    afl::data::HashValue hv(hash);

    // Output with indentation
    {
        afl::io::InternalSink s;
        afl::io::json::Writer wr(s);
        wr.setIndentStep(3);
        wr.visit(&hv);
        a.checkEqual("with-indentation",
                     afl::string::fromBytes(s.getContent()),
                     "{\n"
                     "   \"a\":99,\n"
                     "   \"b\":true\n"
                     "}");
    }

    // Output with line length limit
    {
        afl::io::InternalSink s;
        afl::io::json::Writer wr(s);
        wr.setLineLength(4);
        wr.visit(&hv);
        a.checkEqual("with-limit",
                     afl::string::fromBytes(s.getContent()),
                     "{\"a\":99,\n"
                     "\"b\":true\n"
                     "}");
    }
}
