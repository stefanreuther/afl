/**
  *  \file test/afl/bits/fixedstringtest.cpp
  *  \brief Test for afl::bits::FixedString
  */

#include "afl/bits/fixedstring.hpp"
#include "afl/test/testrunner.hpp"
#include "afl/string/string.hpp"

using afl::base::Bytes_t;
using afl::base::ConstBytes_t;
using afl::string::fromBytes;
using afl::string::toBytes;

/** Test unpackFixedString. */
AFL_TEST("afl.bits.FixedString:unpackFixedString:padded", a) {
    static const uint8_t d[] = { 'a', 'b', 'c', ' ', ' ' };
    a.checkEqual("", fromBytes(afl::bits::unpackFixedString(ConstBytes_t(d))), "abc");
}

AFL_TEST("afl.bits.FixedString:unpackFixedString:full", a) {
    static const uint8_t d[] = { 'a', 'b', 'c', 'd', 'e' };
    a.checkEqual("", fromBytes(afl::bits::unpackFixedString(ConstBytes_t(d))), "abcde");
}

AFL_TEST("afl.bits.FixedString:unpackFixedString:null", a) {
    static const uint8_t d[] = { 'a', 'b', '\0', 'd', 'e' };
    a.checkEqual("", fromBytes(afl::bits::unpackFixedString(ConstBytes_t(d))), "ab");
}

AFL_TEST("afl.bits.FixedString:unpackFixedString:empty", a) {
    static const uint8_t d[] = { ' ', ' ', ' ', ' ', ' ' };
    a.checkEqual("", fromBytes(afl::bits::unpackFixedString(ConstBytes_t(d))), "");
}

AFL_TEST("afl.bits.FixedString:unpackFixedString:empty+null", a) {
    static const uint8_t d[] = { '\0', ' ', ' ', ' ', ' ' };
    a.checkEqual("", fromBytes(afl::bits::unpackFixedString(ConstBytes_t(d))), "");
}

/** Test packFixedString. */
AFL_TEST("afl.bits.FixedString:packFixedString:empty", a) {
    uint8_t tmp[5];
    afl::bits::packFixedString(Bytes_t(tmp), toBytes(""));
    a.checkEqualContent("", ConstBytes_t(tmp), toBytes("     "));
}

AFL_TEST("afl.bits.FixedString:packFixedString:partial", a) {
    uint8_t tmp[5];
    afl::bits::packFixedString(Bytes_t(tmp), toBytes("abc"));
    a.checkEqualContent("", ConstBytes_t(tmp), toBytes("abc  "));
}

AFL_TEST("afl.bits.FixedString:packFixedString:full", a) {
    uint8_t tmp[5];
    afl::bits::packFixedString(Bytes_t(tmp), toBytes("abcde"));
    a.checkEqualContent("", ConstBytes_t(tmp), toBytes("abcde"));
}

AFL_TEST("afl.bits.FixedString:packFixedString:overfull", a) {
    uint8_t tmp[5];
    afl::bits::packFixedString(Bytes_t(tmp), toBytes("yxcvbnm"));
    a.checkEqualContent("", ConstBytes_t(tmp), toBytes("yxcvb"));
}

/** Test the FixedString class. */
AFL_TEST("afl.bits.FixedString:class", a)
{
    static const uint8_t d[3] = {'l','f','m'};
    typedef afl::bits::FixedString<3> FS_t;

    a.checkEqual("unpack", fromBytes(FS_t::unpack(d)), "lfm");

    uint8_t b[3];
    FS_t::pack(b, toBytes("lfm"));
    a.checkEqualContent("pack", ConstBytes_t(b), ConstBytes_t(d));
}
