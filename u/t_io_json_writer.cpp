/**
  *  \file u/t_io_json_writer.cpp
  *  \brief Test for afl::io::json::Writer
  */

#include "afl/io/json/writer.hpp"

#include "u/t_io_json.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/errorvalue.hpp"

namespace {
    String_t formatToString(afl::data::Value* p)
    {
        afl::io::InternalSink s;
        afl::io::json::Writer(s).visit(p);
        return afl::string::fromBytes(s.getContent());
    }
}

/** Simple test. */
void
TestIoJsonWriter::testSimple()
{
    TS_ASSERT_EQUALS(formatToString(0), "null");
    {
        afl::data::BooleanValue bv(true);
        TS_ASSERT_EQUALS(formatToString(&bv), "true");
    }
    {
        afl::data::BooleanValue bv(false);
        TS_ASSERT_EQUALS(formatToString(&bv), "false");
    }
    {
        afl::data::IntegerValue iv(42);
        TS_ASSERT_EQUALS(formatToString(&iv), "42");
    }
    {
        afl::data::IntegerValue iv(-99);
        TS_ASSERT_EQUALS(formatToString(&iv), "-99");
    }
    {
        afl::base::Ref<afl::data::Hash> hash(afl::data::Hash::create());
        hash->setNew("a", new afl::data::IntegerValue(99));
        hash->setNew("b", new afl::data::BooleanValue(true));
        afl::data::HashValue hv(hash);
        TS_ASSERT_EQUALS(formatToString(&hv), "{\"a\":99,\"b\":true}");
    }
    {
        afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
        vec->pushBackNew(new afl::data::IntegerValue(99));
        vec->pushBackNew(new afl::data::BooleanValue(true));
        vec->pushBackNew(0);
        vec->pushBackNew(new afl::data::StringValue("x"));
        afl::data::VectorValue vv(vec);
        TS_ASSERT_EQUALS(formatToString(&vv), "[99,true,null,\"x\"]");
    }
    {
        afl::data::StringValue sv("");
        TS_ASSERT_EQUALS(formatToString(&sv), "\"\"");
    }
    {
        afl::data::StringValue sv("abc");
        TS_ASSERT_EQUALS(formatToString(&sv), "\"abc\"");
    }
    {
        afl::data::StringValue sv(String_t("\n\t\r\0\f\b\033\\\"a", 10));
        TS_ASSERT_EQUALS(formatToString(&sv), "\"\\n\\t\\r\\""u0000\\f\\b\\""u001B\\\\\\\"a\"");
    }
    {
        afl::data::FloatValue fv(1.0);
        TS_ASSERT_EQUALS(formatToString(&fv), "1");
    }
    {
        afl::data::FloatValue fv(-1.0);
        TS_ASSERT_EQUALS(formatToString(&fv), "-1");
    }
    {
        afl::data::FloatValue fv(-1.5);
        TS_ASSERT_EQUALS(formatToString(&fv), "-1.5");
    }
    {
        // FIXME: larger values do not produce exact output:
        //   55.25 --> 55.2500000000000000008673617
        //  255.25 --> 255.2500000000000000008673617
        // Effectively, somewhere in the code, the 2**-60 bit gets unexpectedly added.
        afl::data::FloatValue fv(5.25);
        TS_ASSERT_EQUALS(formatToString(&fv), "5.25");
    }
    {
        afl::data::ErrorValue ev("text", "source");
        TS_ASSERT_EQUALS(formatToString(&ev), "null");
    }
    {
        class Other : public afl::data::Value {
         public:
            virtual void visit(afl::data::Visitor& visitor) const
                { visitor.visitOther(*this); }
            virtual Other* clone() const
                { return new Other(); }
        };
        Other ov;
        TS_ASSERT_EQUALS(formatToString(&ov), "null");
    }
}

/** Test formatted output. */
void
TestIoJsonWriter::testFormat()
{
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
        TS_ASSERT_EQUALS(afl::string::fromBytes(s.getContent()),
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
        TS_ASSERT_EQUALS(afl::string::fromBytes(s.getContent()),
                         "{\"a\":99,\n"
                         "\"b\":true\n"
                         "}");
    }
}
