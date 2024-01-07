/**
  *  \file test/afl/io/resp/writertest.cpp
  *  \brief Test for afl::io::resp::Writer
  */

#include "afl/io/resp/writer.hpp"
#include "afl/test/testrunner.hpp"
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

namespace afl { namespace io { namespace resp {

    /*
     *  Simple serialisation tests (ex TestIoRespWriter::testSimple())
     */

    AFL_TEST("afl.io.resp.Writer:null", a) {
        a.checkEqual("", formatToString(0), "$-1\r\n");
    }

    AFL_TEST("afl.io.resp.Writer:bool:1", a) {
        afl::data::BooleanValue bv(true);
        a.checkEqual("", formatToString(&bv), "$1\r\n1\r\n");
    }

    AFL_TEST("afl.io.resp.Writer:bool:0", a) {
        afl::data::BooleanValue bv(false);
        a.checkEqual("", formatToString(&bv), "$1\r\n0\r\n");
    }

    AFL_TEST("afl.io.resp.Writer:int:pos", a) {
        afl::data::IntegerValue iv(42);
        a.checkEqual("", formatToString(&iv), "$2\r\n42\r\n");
    }

    AFL_TEST("afl.io.resp.Writer:int:neg", a) {
        afl::data::IntegerValue iv(-99);
        a.checkEqual("", formatToString(&iv), "$3\r\n-99\r\n");
    }

    AFL_TEST("afl.io.resp.Writer:hash", a) {
        afl::base::Ref<afl::data::Hash> hash(afl::data::Hash::create());
        hash->setNew("a", new afl::data::IntegerValue(99));
        hash->setNew("b", new afl::data::BooleanValue(true));
        afl::data::HashValue hv(hash);
        a.checkEqual("", formatToString(&hv),
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

    AFL_TEST("afl.io.resp.Writer:vector", a) {
        afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
        vec->pushBackNew(new afl::data::IntegerValue(99));
        vec->pushBackNew(new afl::data::BooleanValue(true));
        vec->pushBackNew(0);
        vec->pushBackNew(new afl::data::StringValue("x"));
        afl::data::VectorValue vv(vec);
        a.checkEqual("", formatToString(&vv),
                     "*4\r\n"
                     "$2\r\n"
                     "99\r\n"
                     "$1\r\n"
                     "1\r\n"
                     "$-1\r\n"
                     "$1\r\n"
                     "x\r\n");
    }

    AFL_TEST("afl.io.resp.Writer:string:empty", a) {
        afl::data::StringValue sv("");
        a.checkEqual("", formatToString(&sv), "$0\r\n\r\n");
    }

    AFL_TEST("afl.io.resp.Writer:string:nonempty", a) {
        afl::data::StringValue sv("abc");
        a.checkEqual("", formatToString(&sv), "$3\r\nabc\r\n");
    }

    /*
     *  Non-object methods (ex TestIoRespWriter::testNonObj)
     */

    AFL_TEST("afl.io.resp.Writer.sendError:single", a) {
        afl::io::InternalSink s;
        Writer(s).sendError("boom");
        a.checkEqual("", afl::string::fromBytes(s.getContent()), "-boom\r\n");
    }

    AFL_TEST("afl.io.resp.Writer.sendError:multi", a) {
        afl::io::InternalSink s;
        Writer(s).sendError("bang\nboom\nbang\n");
        a.checkEqual("", afl::string::fromBytes(s.getContent()), "-bang\r\n");
    }

    AFL_TEST("afl.io.resp.Writer.sendSuccess", a) {
        afl::io::InternalSink s;
        Writer(s).sendSuccess("great!");
        a.checkEqual("", afl::string::fromBytes(s.getContent()), "+great!\r\n");
    }

} } }
