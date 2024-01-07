/**
  *  \file test/afl/data/accesstest.cpp
  *  \brief Test for afl::data::Access
  */

#include "afl/data/access.hpp"

#include <stdexcept>
#include "afl/base/ref.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/data/errorvalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringlist.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/test/testrunner.hpp"

using afl::except::InvalidDataException;
using afl::except::RemoteErrorException;

namespace afl { namespace data {

    /*
     *  Simple tests
     */

    AFL_TEST("afl.data.Access:simple:int:0", a)
    {
        IntegerValue iv0(0);
        a.checkEqual("toInteger",     Access(&iv0).toInteger(), 0);
        a.checkEqual("toString",      Access(&iv0).toString(), "0");
        a.checkEqual("getArraySize",  Access(&iv0).getArraySize(), 0U);
        a.checkEqual("getValue",      Access(&iv0).getValue(), &iv0);
        a.check("isNull", !           Access(&iv0).isNull());
        a.check("char attr isNull",   Access(&iv0)("foo").isNull());
        a.check("str attr isNull",    Access(&iv0)(String_t("foo")).isNull());
        a.check("index isNull",       Access(&iv0)[99].isNull());
        a.check("char attr isNull 2", Access(&iv0)("a").isNull());
    }

    AFL_TEST("afl.data.Access:simple:int:42", a)
    {
        IntegerValue iv1(42);
        a.checkEqual("toInteger",    Access(&iv1).toInteger(), 42);
        a.checkEqual("toString",     Access(&iv1).toString(), "42");
        a.checkEqual("getArraySize", Access(&iv1).getArraySize(), 0U);
        a.checkEqual("getValue",     Access(&iv1).getValue(), &iv1);
        a.check("index isNull",      Access(&iv1)[0].isNull());
        a.check("attr isNull",       Access(&iv1)("a").isNull());

        StringList_t sl;
        Access(&iv1).getHashKeys(sl);
        a.checkEqual("getHashKeys", sl.size(), 0U);

        String_t source, str;
        a.check("isError", !Access(&iv1).isError(source, str));
    }

    AFL_TEST("afl.data.Access:simple:bool:false", a)
    {
        BooleanValue bv0(false);
        a.checkEqual("toInteger",    Access(&bv0).toInteger(), 0);
        a.checkEqual("toString",     Access(&bv0).toString(), "false");
        a.checkEqual("getArraySize", Access(&bv0).getArraySize(), 0U);
        a.check("index isNull",      Access(&bv0)[0].isNull());
        a.check("attr isNull",       Access(&bv0)("a").isNull());
    }

    AFL_TEST("afl.data.Access:simple:bool:true", a)
    {
        BooleanValue bv1(true);
        a.checkEqual("toInteger",    Access(&bv1).toInteger(), 1);
        a.checkEqual("toString",     Access(&bv1).toString(), "true");
        a.checkEqual("getArraySize", Access(&bv1).getArraySize(), 0U);
        a.check("index isNull",      Access(&bv1)[0].isNull());
        a.check("attr isNull",       Access(&bv1)("a").isNull());

        StringList_t sl;
        Access(&bv1).getHashKeys(sl);
        a.checkEqual("getHashKeys", sl.size(), 0U);

        String_t source, str;
        a.check("isError", !Access(&bv1).isError(source, str));
    }

    AFL_TEST("afl.data.Access:simple:str:empty", a)
    {
        StringValue sv0("");
        a.checkEqual("toInteger",    Access(&sv0).toInteger(), 0);
        a.checkEqual("toString",     Access(&sv0).toString(), "");
        a.checkEqual("getArraySize", Access(&sv0).getArraySize(), 0U);
        a.check("index isNull",      Access(&sv0)[0].isNull());
        a.check("attr isNull",       Access(&sv0)("a").isNull());
    }

    AFL_TEST("afl.data.Access:simple:str:zero", a)
    {
        StringValue sv1("0");
        a.checkEqual("toInteger",    Access(&sv1).toInteger(), 0);
        a.checkEqual("toString",     Access(&sv1).toString(), "0");
        a.checkEqual("getArraySize", Access(&sv1).getArraySize(), 0U);
        a.check("index isNull",      Access(&sv1)[0].isNull());
        a.check("attr isNull",       Access(&sv1)("a").isNull());

        StringList_t sl;
        Access(&sv1).getHashKeys(sl);
        a.checkEqual("getHashKeys", sl.size(), 0U);

        String_t source, str;
        a.check("isError", !Access(&sv1).isError(source, str));
    }

    AFL_TEST("afl.data.Access:simple:str:num", a)
    {
        StringValue sv2("123");
        a.checkEqual("toInteger",    Access(&sv2).toInteger(), 123);
        a.checkEqual("toString",     Access(&sv2).toString(), "123");
        a.checkEqual("getArraySize", Access(&sv2).getArraySize(), 0U);
        a.check("index isNull",      Access(&sv2)[0].isNull());
        a.check("attr isNull",       Access(&sv2)("a").isNull());
    }

    AFL_TEST("afl.data.Access:simple:str:alpha", a)
    {
        StringValue sv3("xyz");
        AFL_CHECK_THROWS(a("toInteger"), Access(&sv3).toInteger(), InvalidDataException);
        a.checkEqual("toString",     Access(&sv3).toString(), "xyz");
        a.checkEqual("getArraySize", Access(&sv3).getArraySize(), 0U);
        a.check("index isNull",      Access(&sv3)[0].isNull());
        a.check("attr isNull",       Access(&sv3)("a").isNull());
    }

    AFL_TEST("afl.data.Access:simple:float", a)
    {
        // Try const for a change...
        const FloatValue fv1(3.25);
        a.checkEqual("toInteger",    Access(&fv1).toInteger(), 3);
        a.checkEqual("toString",     Access(&fv1).toString(), "3.25");
        a.checkEqual("getArraySize", Access(&fv1).getArraySize(), 0U);
        a.checkEqual("getValue",     Access(&fv1).getValue(), &fv1);
        a.check("index isNull",      Access(&fv1)[0].isNull());
        a.check("attr isNull",       Access(&fv1)("a").isNull());

        StringList_t sl;
        Access(&fv1).getHashKeys(sl);
        a.checkEqual("getHashKeys", sl.size(), 0U);

        String_t source, str;
        a.check("isError", !Access(&fv1).isError(source, str));
    }

    AFL_TEST("afl.data.Access:simple:null", a)
    {
        a.checkEqual("toInteger",     Access(0).toInteger(), 0);
        a.checkEqual("toString",      Access(0).toString(), "");
        a.checkEqual("getArraySize",  Access(0).getArraySize(), 0U);
        a.check("isNull",             Access(0).isNull());
        a.check("index isNull",       Access(0)[0].isNull());
        a.check("multi-index isNull", Access(0)[0][1][2][3][4][5].isNull());
        a.check("attr isNull",        Access(0)("a").isNull());
        a.check("multi-attr isNull",  Access(0)("a")("b")("c")("d").isNull());

        StringList_t sl;
        Access(0).getHashKeys(sl);
        a.checkEqual("getHashKeys", sl.size(), 0U);

        String_t source, str;
        a.check("isError", !Access(0).isError(source, str));
    }

    /*
     *  Test accessing a vector
     */

    AFL_TEST("afl.data.Access:vector", a)
    {
        // Make a vector
        afl::base::Ref<Vector> vec(Vector::create());
        vec->pushBackNew(new IntegerValue(1));
        vec->pushBackNew(0);
        vec->pushBackNew(new IntegerValue(42));
        vec->pushBackNew(new StringValue("xyz"));
        vec->pushBackNew(new StringValue("abc"));
        vec->pushBackNew(new IntegerValue(7));
        vec->pushBackNew(new StringValue("9"));
        VectorValue vv(vec);

        // Access it
        AFL_CHECK_THROWS(a("toInteger"), (void)+Access(&vv).toInteger(), InvalidDataException);
        a.checkEqual("toString",     Access(&vv).toString(), "1,,42,xyz,abc,7,9");
        a.checkEqual("getArraySize", Access(&vv).getArraySize(), 7U);
        a.check("isNull",           !Access(&vv).isNull());
        a.checkEqual("getValue",     Access(&vv).getValue(), &vv);

        a.checkEqual("index 0 toInteger", Access(&vv)[0].toInteger(), 1);
        a.checkEqual("index 1 toInteger", Access(&vv)[1].toInteger(), 0);
        a.checkEqual("index 2 toInteger", Access(&vv)[2].toInteger(), 42);
        AFL_CHECK_THROWS(a("index 3 toInteger"), (void)+Access(&vv)[3].toInteger(), InvalidDataException);
        AFL_CHECK_THROWS(a("index 4 toInteger"), (void)+Access(&vv)[4].toInteger(), InvalidDataException);
        a.checkEqual("index 5 toInteger", Access(&vv)[5].toInteger(), 7);
        a.checkEqual("index 6 toInteger", Access(&vv)[6].toInteger(), 9);

        a.checkEqual("index 0 toString", Access(&vv)[0].toString(), "1");
        a.checkEqual("index 1 toString", Access(&vv)[1].toString(), "");
        a.checkEqual("index 2 toString", Access(&vv)[2].toString(), "42");
        a.checkEqual("index 3 toString", Access(&vv)[3].toString(), "xyz");
        a.checkEqual("index 4 toString", Access(&vv)[4].toString(), "abc");
        a.checkEqual("index 5 toString", Access(&vv)[5].toString(), "7");
        a.checkEqual("index 6 toString", Access(&vv)[6].toString(), "9");

        a.checkEqual("attr 1 toInteger",   Access(&vv)("1").toInteger(), 0);
        a.check("attr 1 isNull",           Access(&vv)("1").isNull());
        a.checkEqual("attr 2 toInteger",   Access(&vv)("2").toInteger(), 0);
        a.check("attr 2 isNull",           Access(&vv)("2").isNull());
        a.checkEqual("attr 42 toString",   Access(&vv)("42").toString(), "xyz");
        a.checkEqual("attr abc toInteger", Access(&vv)("abc").toInteger(), 7);
        a.check("attr 9 isNull",           Access(&vv)("9").isNull());
        a.check("attr 042 isNull",         Access(&vv)("042").isNull());

        // 7 elements = 3 complete pairs
        StringList_t sl;
        Access(&vv).getHashKeys(sl);
        a.checkEqual("getHashKeys size", sl.size(), 3U);
        a.checkEqual("getHashKeys 0", sl[0], "1");
        a.checkEqual("getHashKeys 1", sl[1], "42");
        a.checkEqual("getHashKeys 2", sl[2], "abc");
    }

    /*
     *  Test accessing a hash.
     */

    AFL_TEST("afl.data.Access:hash", a)
    {
        // Make a hash
        afl::base::Ref<Hash> hash(Hash::create());
        hash->setNew("1", new IntegerValue(1));
        hash->setNew("xyz", new IntegerValue(42));
        hash->setNew("01", new StringValue("77"));
        HashValue hv(hash);

        // Access it
        AFL_CHECK_THROWS(a("toInteger"), (void)+Access(&hv).toInteger(), InvalidDataException);
        a.checkEqual("toString",     Access(&hv).toString(), "<hash>");
        a.checkEqual("getArraySize", Access(&hv).getArraySize(), 0U);
        a.check("isNull",           !Access(&hv).isNull());
        a.checkEqual("getValue",     Access(&hv).getValue(), &hv);

        a.check("index 0 isNull", Access(&hv)[0].isNull());
        a.check("index 1 isNull", Access(&hv)[1].isNull()); // FIXME: this could change in the future to return IntegerValue(1)

        a.checkEqual("attr 1 toInteger",        Access(&hv)("1").toInteger(), 1);
        a.checkEqual("attr xyz toInteger",      Access(&hv)("xyz").toInteger(), 42);
        a.checkEqual("attr 01 toInteger",       Access(&hv)("01").toInteger(), 77);
        a.checkEqual("attr whatever toInteger", Access(&hv)("whatever").toInteger(), 0);

        // Get keys
        StringList_t sl;
        Access(&hv).getHashKeys(sl);
        a.checkEqual("getHashKeys size", sl.size(), 3U);
        a.checkEqual("getHashKeys 0", sl[0], "1");
        a.checkEqual("getHashKeys 1", sl[1], "xyz");
        a.checkEqual("getHashKeys 2", sl[2], "01");
    }

    /*
     *  Test accessing an error.
     */

    AFL_TEST("afl.data.Access:error", a)
    {
        ErrorValue ev("src", "hi");
        Access aev(&ev);
        AFL_CHECK_THROWS(a("toInteger"),    aev.toInteger(),    RemoteErrorException);
        AFL_CHECK_THROWS(a("toString"),     aev.toString(),     RemoteErrorException);
        AFL_CHECK_THROWS(a("getArraySize"), aev.getArraySize(), RemoteErrorException);
        AFL_CHECK_THROWS(a("attr"),         aev("x"),           RemoteErrorException);
        AFL_CHECK_THROWS(a("index"),        aev[0],             RemoteErrorException);

        // Can extract the error
        String_t s, src;
        a.check("isError", aev.isError(src, s));
        a.checkEqual("isError str", s, "hi");
        a.checkEqual("isError src", src, "src");

        // It's not null
        a.check("isNull", !aev.isNull());
    }

    /*
     *  Test list operations.
     */
    AFL_TEST("afl.data.Access:toList", a)
    {
        // Make a vector
        afl::base::Ref<Vector> vec(Vector::create());
        vec->pushBackNew(new IntegerValue(1));
        vec->pushBackNew(0);
        vec->pushBackNew(new IntegerValue(42));
        vec->pushBackNew(new StringValue("-9"));
        vec->pushBackNew(new StringValue("77"));
        vec->pushBackNew(new IntegerValue(7));
        vec->pushBackNew(new StringValue("9"));
        VectorValue vv(vec);

        // Convert to string list
        StringList_t slist;
        Access(&vv).toStringList(slist);
        a.checkEqual("toStringList size", slist.size(), 7U);
        a.checkEqual("toStringList 0", slist[0], "1");
        a.checkEqual("toStringList 1", slist[1], "");
        a.checkEqual("toStringList 2", slist[2], "42");
        a.checkEqual("toStringList 3", slist[3], "-9");
        a.checkEqual("toStringList 4", slist[4], "77");
        a.checkEqual("toStringList 5", slist[5], "7");
        a.checkEqual("toStringList 6", slist[6], "9");

        // Convert again (check that it appends)
        Access(&vv).toStringList(slist);
        a.checkEqual("toStringList size 2", slist.size(), 14U);
        for (size_t i = 0; i < 7; ++i) {
            a.checkEqual("toStringList content", slist[i], slist[i+7]);
        }

        // Same thing for integer list
        IntegerList_t ilist;
        Access(&vv).toIntegerList(ilist);
        a.checkEqual("toIntegerList size", ilist.size(), 7U);
        a.checkEqual("toIntegerList 0", ilist[0], 1);
        a.checkEqual("toIntegerList 1", ilist[1], 0);
        a.checkEqual("toIntegerList 2", ilist[2], 42);
        a.checkEqual("toIntegerList 3", ilist[3], -9);
        a.checkEqual("toIntegerList 4", ilist[4], 77);
        a.checkEqual("toIntegerList 5", ilist[5], 7);
        a.checkEqual("toIntegerList 6", ilist[6], 9);

        // Convert again (check that it appends)
        Access(&vv).toIntegerList(ilist);
        a.checkEqual("toIntegerList size 2", ilist.size(), 14U);
        for (size_t i = 0; i < 7; ++i) {
            a.checkEqual("toIntegerList content", ilist[i], ilist[i+7]);
        }
    }

} }
