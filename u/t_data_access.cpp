/**
  *  \file u/t_data_access.cpp
  *  \brief Test for afl::data::Access
  */

#include <stdexcept>
#include "afl/data/access.hpp"

#include "u/t_data.hpp"
#include "afl/base/ptr.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/data/errorvalue.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/data/stringlist.hpp"
#include "afl/data/floatvalue.hpp"

/** Simple tests. */
void
TestDataAccess::testSimple()
{
    using afl::data::Access;
    {
        afl::data::IntegerValue iv0(0);
        TS_ASSERT_EQUALS(Access(&iv0).toInteger(), 0);
        TS_ASSERT_EQUALS(Access(&iv0).toString(), "0");
        TS_ASSERT_EQUALS(Access(&iv0).getArraySize(), 0U);
        TS_ASSERT_EQUALS(Access(&iv0).getValue(), &iv0);
        TS_ASSERT(!Access(&iv0).isNull());
        TS_ASSERT(Access(&iv0)("foo").isNull());
        TS_ASSERT(Access(&iv0)(String_t("foo")).isNull());
        TS_ASSERT(Access(&iv0)[99].isNull());
        TS_ASSERT(Access(&iv0)("a").isNull());
    }
    {
        afl::data::IntegerValue iv1(42);
        TS_ASSERT_EQUALS(Access(&iv1).toInteger(), 42);
        TS_ASSERT_EQUALS(Access(&iv1).toString(), "42");
        TS_ASSERT_EQUALS(Access(&iv1).getArraySize(), 0U);
        TS_ASSERT_EQUALS(Access(&iv1).getValue(), &iv1);
        TS_ASSERT(Access(&iv1)[0].isNull());
        TS_ASSERT(Access(&iv1)("a").isNull());

        afl::data::StringList_t sl;
        Access(&iv1).getHashKeys(sl);
        TS_ASSERT_EQUALS(sl.size(), 0U);

        String_t source, str;
        TS_ASSERT(!Access(&iv1).isError(source, str));
    }
    {
        afl::data::BooleanValue bv0(false);
        TS_ASSERT_EQUALS(Access(&bv0).toInteger(), 0);
        TS_ASSERT_EQUALS(Access(&bv0).toString(), "false");
        TS_ASSERT_EQUALS(Access(&bv0).getArraySize(), 0U);
        TS_ASSERT(Access(&bv0)[0].isNull());
        TS_ASSERT(Access(&bv0)("a").isNull());
    }
    {
        afl::data::BooleanValue bv1(true);
        TS_ASSERT_EQUALS(Access(&bv1).toInteger(), 1);
        TS_ASSERT_EQUALS(Access(&bv1).toString(), "true");
        TS_ASSERT_EQUALS(Access(&bv1).getArraySize(), 0U);
        TS_ASSERT(Access(&bv1)[0].isNull());
        TS_ASSERT(Access(&bv1)("a").isNull());

        afl::data::StringList_t sl;
        Access(&bv1).getHashKeys(sl);
        TS_ASSERT_EQUALS(sl.size(), 0U);

        String_t source, str;
        TS_ASSERT(!Access(&bv1).isError(source, str));
    }
    {
        afl::data::StringValue sv0("");
        TS_ASSERT_EQUALS(Access(&sv0).toInteger(), 0);
        TS_ASSERT_EQUALS(Access(&sv0).toString(), "");
        TS_ASSERT_EQUALS(Access(&sv0).getArraySize(), 0U);
        TS_ASSERT(Access(&sv0)[0].isNull());
        TS_ASSERT(Access(&sv0)("a").isNull());
    }
    {
        afl::data::StringValue sv1("0");
        TS_ASSERT_EQUALS(Access(&sv1).toInteger(), 0);
        TS_ASSERT_EQUALS(Access(&sv1).toString(), "0");
        TS_ASSERT_EQUALS(Access(&sv1).getArraySize(), 0U);
        TS_ASSERT(Access(&sv1)[0].isNull());
        TS_ASSERT(Access(&sv1)("a").isNull());

        afl::data::StringList_t sl;
        Access(&sv1).getHashKeys(sl);
        TS_ASSERT_EQUALS(sl.size(), 0U);

        String_t source, str;
        TS_ASSERT(!Access(&sv1).isError(source, str));
    }
    {
        afl::data::StringValue sv2("123");
        TS_ASSERT_EQUALS(Access(&sv2).toInteger(), 123);
        TS_ASSERT_EQUALS(Access(&sv2).toString(), "123");
        TS_ASSERT_EQUALS(Access(&sv2).getArraySize(), 0U);
        TS_ASSERT(Access(&sv2)[0].isNull());
        TS_ASSERT(Access(&sv2)("a").isNull());
    }
    {
        afl::data::StringValue sv3("xyz");
        TS_ASSERT_THROWS(Access(&sv3).toInteger(), afl::except::InvalidDataException);
        TS_ASSERT_EQUALS(Access(&sv3).toString(), "xyz");
        TS_ASSERT_EQUALS(Access(&sv3).getArraySize(), 0U);
        TS_ASSERT(Access(&sv3)[0].isNull());
        TS_ASSERT(Access(&sv3)("a").isNull());
    }
    {
        // Try const for a change...
        const afl::data::FloatValue fv1(3.25);
        TS_ASSERT_EQUALS(Access(&fv1).toInteger(), 3);
        TS_ASSERT_EQUALS(Access(&fv1).toString(), "3.25");
        TS_ASSERT_EQUALS(Access(&fv1).getArraySize(), 0U);
        TS_ASSERT_EQUALS(Access(&fv1).getValue(), &fv1);
        TS_ASSERT(Access(&fv1)[0].isNull());
        TS_ASSERT(Access(&fv1)("a").isNull());

        afl::data::StringList_t sl;
        Access(&fv1).getHashKeys(sl);
        TS_ASSERT_EQUALS(sl.size(), 0U);

        String_t source, str;
        TS_ASSERT(!Access(&fv1).isError(source, str));
    }
    {
        TS_ASSERT_EQUALS(Access(0).toInteger(), 0);
        TS_ASSERT_EQUALS(Access(0).toString(), "");
        TS_ASSERT_EQUALS(Access(0).getArraySize(), 0U);
        TS_ASSERT(Access(0).isNull());
        TS_ASSERT(Access(0)[0].isNull());
        TS_ASSERT(Access(0)[0][1][2][3][4][5].isNull());
        TS_ASSERT(Access(0)("a").isNull());
        TS_ASSERT(Access(0)("a")("b")("c")("d").isNull());

        afl::data::StringList_t sl;
        Access(0).getHashKeys(sl);
        TS_ASSERT_EQUALS(sl.size(), 0U);

        String_t source, str;
        TS_ASSERT(!Access(0).isError(source, str));
    }
}

/** Test accessing a vector. */
void
TestDataAccess::testVector()
{
    // Make a vector
    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(1));
    vec->pushBackNew(0);
    vec->pushBackNew(new afl::data::IntegerValue(42));
    vec->pushBackNew(new afl::data::StringValue("xyz"));
    vec->pushBackNew(new afl::data::StringValue("abc"));
    vec->pushBackNew(new afl::data::IntegerValue(7));
    vec->pushBackNew(new afl::data::StringValue("9"));
    afl::data::VectorValue vv(vec);

    // Access it
    using afl::data::Access;
    TS_ASSERT_THROWS((void)+Access(&vv).toInteger(), afl::except::InvalidDataException);
    TS_ASSERT_EQUALS(Access(&vv).toString(), "1,,42,xyz,abc,7,9");
    TS_ASSERT_EQUALS(Access(&vv).getArraySize(), 7U);
    TS_ASSERT(!Access(&vv).isNull());
    TS_ASSERT_EQUALS(Access(&vv).getValue(), &vv);

    TS_ASSERT_EQUALS(Access(&vv)[0].toInteger(), 1);
    TS_ASSERT_EQUALS(Access(&vv)[1].toInteger(), 0);
    TS_ASSERT_EQUALS(Access(&vv)[2].toInteger(), 42);
    TS_ASSERT_THROWS((void)+Access(&vv)[3].toInteger(), afl::except::InvalidDataException);
    TS_ASSERT_THROWS((void)+Access(&vv)[4].toInteger(), afl::except::InvalidDataException);
    TS_ASSERT_EQUALS(Access(&vv)[5].toInteger(), 7);
    TS_ASSERT_EQUALS(Access(&vv)[6].toInteger(), 9);

    TS_ASSERT_EQUALS(Access(&vv)[0].toString(), "1");
    TS_ASSERT_EQUALS(Access(&vv)[1].toString(), "");
    TS_ASSERT_EQUALS(Access(&vv)[2].toString(), "42");
    TS_ASSERT_EQUALS(Access(&vv)[3].toString(), "xyz");
    TS_ASSERT_EQUALS(Access(&vv)[4].toString(), "abc");
    TS_ASSERT_EQUALS(Access(&vv)[5].toString(), "7");
    TS_ASSERT_EQUALS(Access(&vv)[6].toString(), "9");

    TS_ASSERT_EQUALS(Access(&vv)("1").toInteger(), 0);
    TS_ASSERT(Access(&vv)("1").isNull());
    TS_ASSERT_EQUALS(Access(&vv)("2").toInteger(), 0);
    TS_ASSERT(Access(&vv)("2").isNull());
    TS_ASSERT_EQUALS(Access(&vv)("42").toString(), "xyz");
    TS_ASSERT_EQUALS(Access(&vv)("abc").toInteger(), 7);
    TS_ASSERT(Access(&vv)("9").isNull());
    TS_ASSERT(Access(&vv)("042").isNull());

    // 7 elements = 3 complete pairs
    afl::data::StringList_t sl;
    Access(&vv).getHashKeys(sl);
    TS_ASSERT_EQUALS(sl.size(), 3U);
    TS_ASSERT_EQUALS(sl[0], "1");
    TS_ASSERT_EQUALS(sl[1], "42");
    TS_ASSERT_EQUALS(sl[2], "abc");
}

/** Test accessing a hash. */
void
TestDataAccess::testHash()
{
    // Make a hash
    afl::base::Ref<afl::data::Hash> hash(afl::data::Hash::create());
    hash->setNew("1", new afl::data::IntegerValue(1));
    hash->setNew("xyz", new afl::data::IntegerValue(42));
    hash->setNew("01", new afl::data::StringValue("77"));
    afl::data::HashValue hv(hash);

    // Access it
    using afl::data::Access;
    TS_ASSERT_THROWS((void)+Access(&hv).toInteger(), afl::except::InvalidDataException);
    TS_ASSERT_EQUALS(Access(&hv).toString(), "<hash>");
    TS_ASSERT_EQUALS(Access(&hv).getArraySize(), 0U);
    TS_ASSERT(!Access(&hv).isNull());
    TS_ASSERT_EQUALS(Access(&hv).getValue(), &hv);

    TS_ASSERT(Access(&hv)[0].isNull());
    TS_ASSERT(Access(&hv)[1].isNull()); // FIXME: this could change in the future to return IntegerValue(1)

    TS_ASSERT_EQUALS(Access(&hv)("1").toInteger(), 1);
    TS_ASSERT_EQUALS(Access(&hv)("xyz").toInteger(), 42);
    TS_ASSERT_EQUALS(Access(&hv)("01").toInteger(), 77);
    TS_ASSERT_EQUALS(Access(&hv)("whatever").toInteger(), 0);

    // Get keys
    afl::data::StringList_t sl;
    Access(&hv).getHashKeys(sl);
    TS_ASSERT_EQUALS(sl.size(), 3U);
    TS_ASSERT_EQUALS(sl[0], "1");
    TS_ASSERT_EQUALS(sl[1], "xyz");
    TS_ASSERT_EQUALS(sl[2], "01");
}

/** Test accessing an error. */
void
TestDataAccess::testError()
{
    using afl::data::Access;
    afl::data::ErrorValue ev("src", "hi");
    Access aev(&ev);
    TS_ASSERT_THROWS(aev.toInteger(), afl::except::RemoteErrorException);
    TS_ASSERT_THROWS(aev.toString(), afl::except::RemoteErrorException);
    TS_ASSERT_THROWS(aev.getArraySize(), afl::except::RemoteErrorException);
    TS_ASSERT_THROWS(aev("x"), afl::except::RemoteErrorException);
    TS_ASSERT_THROWS(aev[0], afl::except::RemoteErrorException);

    // Can extract the error
    String_t s, src;
    TS_ASSERT(aev.isError(src, s));
    TS_ASSERT_EQUALS(s, "hi");
    TS_ASSERT_EQUALS(src, "src");

    // It's not null
    TS_ASSERT(!aev.isNull());
}

/** Test list operations. */
void
TestDataAccess::testList()
{
    using afl::data::Access;

    // Make a vector
    afl::base::Ref<afl::data::Vector> vec(afl::data::Vector::create());
    vec->pushBackNew(new afl::data::IntegerValue(1));
    vec->pushBackNew(0);
    vec->pushBackNew(new afl::data::IntegerValue(42));
    vec->pushBackNew(new afl::data::StringValue("-9"));
    vec->pushBackNew(new afl::data::StringValue("77"));
    vec->pushBackNew(new afl::data::IntegerValue(7));
    vec->pushBackNew(new afl::data::StringValue("9"));
    afl::data::VectorValue vv(vec);

    // Convert to string list
    afl::data::StringList_t slist;
    Access(&vv).toStringList(slist);
    TS_ASSERT_EQUALS(slist.size(), 7U);
    TS_ASSERT_EQUALS(slist[0], "1");
    TS_ASSERT_EQUALS(slist[1], "");
    TS_ASSERT_EQUALS(slist[2], "42");
    TS_ASSERT_EQUALS(slist[3], "-9");
    TS_ASSERT_EQUALS(slist[4], "77");
    TS_ASSERT_EQUALS(slist[5], "7");
    TS_ASSERT_EQUALS(slist[6], "9");

    // Convert again (check that it appends)
    Access(&vv).toStringList(slist);
    TS_ASSERT_EQUALS(slist.size(), 14U);
    for (size_t i = 0; i < 7; ++i) {
        TS_ASSERT_EQUALS(slist[i], slist[i+7]);
    }

    // Same thing for integer list
    afl::data::IntegerList_t ilist;
    Access(&vv).toIntegerList(ilist);
    TS_ASSERT_EQUALS(ilist.size(), 7U);
    TS_ASSERT_EQUALS(ilist[0], 1);
    TS_ASSERT_EQUALS(ilist[1], 0);
    TS_ASSERT_EQUALS(ilist[2], 42);
    TS_ASSERT_EQUALS(ilist[3], -9);
    TS_ASSERT_EQUALS(ilist[4], 77);
    TS_ASSERT_EQUALS(ilist[5], 7);
    TS_ASSERT_EQUALS(ilist[6], 9);

    // Convert again (check that it appends)
    Access(&vv).toIntegerList(ilist);
    TS_ASSERT_EQUALS(ilist.size(), 14U);
    for (size_t i = 0; i < 7; ++i) {
        TS_ASSERT_EQUALS(ilist[i], ilist[i+7]);
    }
}
