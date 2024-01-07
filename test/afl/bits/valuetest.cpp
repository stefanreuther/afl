/**
  *  \file test/afl/bits/valuetest.cpp
  *  \brief Test for afl::bits::Value
  */

#include "afl/bits/value.hpp"

#include "afl/test/testrunner.hpp"
#include "afl/bits/int8.hpp"
#include "afl/bits/int16le.hpp"
#include "afl/bits/int32le.hpp"
#include "afl/bits/int64le.hpp"
#include "afl/bits/uint8.hpp"
#include "afl/bits/uint16le.hpp"
#include "afl/bits/uint32le.hpp"
#include "afl/bits/uint64le.hpp"

/** Test some arithmetic operations. */
AFL_TEST("afl.bits.Value:expr", a) {
    // Create an element and initialize it
    afl::bits::Value<afl::bits::Int16LE> val;
    val = 0;

    a.checkEqual("01. sizeof", sizeof(val), 2U);
    a.checkEqual("02. value", val.m_bytes[0], 0U);
    a.checkEqual("03. value", val.m_bytes[1], 0U);

    // Some computation
    val = static_cast<int16_t>(val + 42);
    a.checkEqual("11. value", val.m_bytes[0], 42U);
    a.checkEqual("12. value", val.m_bytes[1], 0U);

    val = static_cast<int16_t>(val * 10);
    a.checkEqual("21. value", val.m_bytes[0], 164U);
    a.checkEqual("22. value", val.m_bytes[1], 1U);

    int16_t v = val;
    a.checkEqual("23. value", v, 420);
}

/** Test different signed types. */
AFL_TEST("afl.bits.Value:Int8", a) {
    afl::bits::Value<afl::bits::Int8> val;
    a.checkEqual("01", sizeof(val), 1U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    val = 0x12;
    a.checkEqual("21", val.m_bytes[0], 0x12U);
    val = -3;
    a.checkEqual("31", val.m_bytes[0], 0xFDU);
}

AFL_TEST("afl.bits.Value:Int16", a) {
    afl::bits::Value<afl::bits::Int16LE> val;
    a.checkEqual("01", sizeof(val), 2U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    a.checkEqual("12", val.m_bytes[1], 0U);
    val = 0x1234;
    a.checkEqual("21", val.m_bytes[0], 0x34U);
    a.checkEqual("22", val.m_bytes[1], 0x12U);
    val = -3;
    a.checkEqual("31", val.m_bytes[0], 0xFDU);
    a.checkEqual("32", val.m_bytes[1], 0xFFU);
}

AFL_TEST("afl.bits.Value:Int32", a) {
    afl::bits::Value<afl::bits::Int32LE> val;
    a.checkEqual("01", sizeof(val), 4U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    a.checkEqual("12", val.m_bytes[1], 0U);
    a.checkEqual("13", val.m_bytes[2], 0U);
    a.checkEqual("14", val.m_bytes[3], 0U);
    val = 0x12345678;
    a.checkEqual("21", val.m_bytes[0], 0x78U);
    a.checkEqual("22", val.m_bytes[1], 0x56U);
    a.checkEqual("23", val.m_bytes[2], 0x34U);
    a.checkEqual("24", val.m_bytes[3], 0x12U);
    val = -3;
    a.checkEqual("31", val.m_bytes[0], 0xFDU);
    a.checkEqual("32", val.m_bytes[1], 0xFFU);
    a.checkEqual("33", val.m_bytes[2], 0xFFU);
    a.checkEqual("34", val.m_bytes[3], 0xFFU);
}

AFL_TEST("afl.bits.Value:Int64", a) {
    afl::bits::Value<afl::bits::Int64LE> val;
    a.checkEqual("01", sizeof(val), 8U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    a.checkEqual("12", val.m_bytes[1], 0U);
    a.checkEqual("13", val.m_bytes[2], 0U);
    a.checkEqual("14", val.m_bytes[3], 0U);
    a.checkEqual("15", val.m_bytes[4], 0U);
    a.checkEqual("16", val.m_bytes[5], 0U);
    a.checkEqual("17", val.m_bytes[6], 0U);
    a.checkEqual("18", val.m_bytes[7], 0U);
    val = 0x123456789ABCDEF0LL;
    a.checkEqual("21", val.m_bytes[0], 0xF0U);
    a.checkEqual("22", val.m_bytes[1], 0xDEU);
    a.checkEqual("23", val.m_bytes[2], 0xBCU);
    a.checkEqual("24", val.m_bytes[3], 0x9AU);
    a.checkEqual("25", val.m_bytes[4], 0x78U);
    a.checkEqual("26", val.m_bytes[5], 0x56U);
    a.checkEqual("27", val.m_bytes[6], 0x34U);
    a.checkEqual("28", val.m_bytes[7], 0x12U);
    val = -3;
    a.checkEqual("31", val.m_bytes[0], 0xFDU);
    a.checkEqual("32", val.m_bytes[1], 0xFFU);
    a.checkEqual("33", val.m_bytes[2], 0xFFU);
    a.checkEqual("34", val.m_bytes[3], 0xFFU);
    a.checkEqual("35", val.m_bytes[4], 0xFFU);
    a.checkEqual("36", val.m_bytes[5], 0xFFU);
    a.checkEqual("37", val.m_bytes[6], 0xFFU);
    a.checkEqual("38", val.m_bytes[7], 0xFFU);
}

/** Test different unsigned types. */
AFL_TEST("afl.bits.Value:UInt8", a) {
    afl::bits::Value<afl::bits::UInt8> val;
    a.checkEqual("01", sizeof(val), 1U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    val = 0x12;
    a.checkEqual("21", val.m_bytes[0], 0x12U);
}

AFL_TEST("afl.bits.Value:UInt16", a) {
    afl::bits::Value<afl::bits::UInt16LE> val;
    a.checkEqual("01", sizeof(val), 2U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    a.checkEqual("12", val.m_bytes[1], 0U);
    val = 0x1234;
    a.checkEqual("21", val.m_bytes[0], 0x34U);
    a.checkEqual("22", val.m_bytes[1], 0x12U);
}

AFL_TEST("afl.bits.Value:UInt32", a) {
    afl::bits::Value<afl::bits::UInt32LE> val;
    a.checkEqual("01", sizeof(val), 4U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    a.checkEqual("12", val.m_bytes[1], 0U);
    a.checkEqual("13", val.m_bytes[2], 0U);
    a.checkEqual("14", val.m_bytes[3], 0U);
    val = 0x12345678;
    a.checkEqual("21", val.m_bytes[0], 0x78U);
    a.checkEqual("22", val.m_bytes[1], 0x56U);
    a.checkEqual("23", val.m_bytes[2], 0x34U);
    a.checkEqual("24", val.m_bytes[3], 0x12U);
}

AFL_TEST("afl.bits.Value:UInt64", a) {
    afl::bits::Value<afl::bits::UInt64LE> val;
    a.checkEqual("01", sizeof(val), 8U);
    val = 0;
    a.checkEqual("11", val.m_bytes[0], 0U);
    a.checkEqual("12", val.m_bytes[1], 0U);
    a.checkEqual("13", val.m_bytes[2], 0U);
    a.checkEqual("14", val.m_bytes[3], 0U);
    a.checkEqual("15", val.m_bytes[4], 0U);
    a.checkEqual("16", val.m_bytes[5], 0U);
    a.checkEqual("17", val.m_bytes[6], 0U);
    a.checkEqual("18", val.m_bytes[7], 0U);
    val = 0x123456789ABCDEF0ULL;
    a.checkEqual("21", val.m_bytes[0], 0xF0U);
    a.checkEqual("22", val.m_bytes[1], 0xDEU);
    a.checkEqual("23", val.m_bytes[2], 0xBCU);
    a.checkEqual("24", val.m_bytes[3], 0x9AU);
    a.checkEqual("25", val.m_bytes[4], 0x78U);
    a.checkEqual("26", val.m_bytes[5], 0x56U);
    a.checkEqual("27", val.m_bytes[6], 0x34U);
    a.checkEqual("28", val.m_bytes[7], 0x12U);
}

/** Test structure layout. */
AFL_TEST("afl.bits.Value:structure", a)
{
    // One of the goals of afl::bits::Value is the ability to
    // efficiently build structures for a given on-disk layout.
    // We expect this structure to have a fixed size:
    struct SomeStruct {
        afl::bits::Value<afl::bits::Int16LE> m_one;         // 2 bytes
        char m_two[3];                                      // 3 bytes
        afl::bits::Value<afl::bits::Int16LE> m_three;       // 2 bytes
        afl::bits::Value<afl::bits::Int32LE> m_four;        // 4 bytes
    };
    a.checkEqual("", sizeof(SomeStruct), 11U);
}
