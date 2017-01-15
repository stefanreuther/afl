/**
  *  \file u/t_io_resp_writer.cpp
  *  \brief Test for afl::io::resp::Writer
  */

#include "afl/io/resp/writer.hpp"

#include "u/t_io_resp.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/io/internalsink.hpp"

namespace {
    String_t formatToString(afl::data::Value* p)
    {
        afl::io::InternalSink s;
        afl::io::resp::Writer(s).visit(p);
        return afl::string::fromBytes(s.getContent());
    }
}

/** Test simple invocations. */
void
TestIoRespWriter::testSimple()
{
    TS_ASSERT_EQUALS(formatToString(0), "$-1\r\n");
    {
        afl::data::BooleanValue bv(true);
        TS_ASSERT_EQUALS(formatToString(&bv), "$1\r\n1\r\n");
    }
    {
        afl::data::BooleanValue bv(false);
        TS_ASSERT_EQUALS(formatToString(&bv), "$1\r\n0\r\n");
    }
    {
        afl::data::IntegerValue iv(42);
        TS_ASSERT_EQUALS(formatToString(&iv), "$2\r\n42\r\n");
    }
    {
        afl::data::IntegerValue iv(-99);
        TS_ASSERT_EQUALS(formatToString(&iv), "$3\r\n-99\r\n");
    }
    {
        afl::base::Ref<afl::data::Hash> hash(afl::data::Hash::create());
        hash->setNew("a", new afl::data::IntegerValue(99));
        hash->setNew("b", new afl::data::BooleanValue(true));
        afl::data::HashValue hv(hash);
        TS_ASSERT_EQUALS(formatToString(&hv),
                         "*4\r\n"
                         "$1\r\n"
                         "a\r\n"
                         "$2\r\n"
                         "99\r\n"
                         "$1\r\n"
                         "b\r\n"
                         "$1\r\n"
                         "1\r\n");
    }
    {
        afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
        vec->pushBackNew(new afl::data::IntegerValue(99));
        vec->pushBackNew(new afl::data::BooleanValue(true));
        vec->pushBackNew(0);
        vec->pushBackNew(new afl::data::StringValue("x"));
        afl::data::VectorValue vv(vec);
        TS_ASSERT_EQUALS(formatToString(&vv),
                         "*4\r\n"
                         "$2\r\n"
                         "99\r\n"
                         "$1\r\n"
                         "1\r\n"
                         "$-1\r\n"
                         "$1\r\n"
                         "x\r\n");
    }
    {
        afl::data::StringValue sv("");
        TS_ASSERT_EQUALS(formatToString(&sv), "$0\r\n\r\n");
    }
    {
        afl::data::StringValue sv("abc");
        TS_ASSERT_EQUALS(formatToString(&sv), "$3\r\nabc\r\n");
    }
}

/** Test non-object API. */
void
TestIoRespWriter::testNonObj()
{
    {
        afl::io::InternalSink s;
        afl::io::resp::Writer(s).sendError("boom");
        TS_ASSERT_EQUALS(afl::string::fromBytes(s.getContent()), "-boom\r\n");
    }
    {
        afl::io::InternalSink s;
        afl::io::resp::Writer(s).sendError("bang\nboom\nbang\n");
        TS_ASSERT_EQUALS(afl::string::fromBytes(s.getContent()), "-bang\r\n");
    }
    {
        afl::io::InternalSink s;
        afl::io::resp::Writer(s).sendSuccess("great!");
        TS_ASSERT_EQUALS(afl::string::fromBytes(s.getContent()), "+great!\r\n");
    }
}
