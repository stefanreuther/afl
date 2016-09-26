/**
  *  \file u/t_bits_value.cpp
  *  \brief Test for afl::bits::Value
  */

#include "afl/bits/value.hpp"

#include "u/t_bits.hpp"

#include "afl/bits/int8.hpp"
#include "afl/bits/int16le.hpp"
#include "afl/bits/int32le.hpp"
#include "afl/bits/int64le.hpp"
#include "afl/bits/uint8.hpp"
#include "afl/bits/uint16le.hpp"
#include "afl/bits/uint32le.hpp"
#include "afl/bits/uint64le.hpp"

/** Test some arithmetic operations. */
void
TestBitsValue::testExpr()
{
    // Create an element and initialize it
    afl::bits::Value<afl::bits::Int16LE> val;
    val = 0;

    TS_ASSERT_EQUALS(sizeof(val), 2U);
    TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
    TS_ASSERT_EQUALS(val.m_bytes[1], 0U);

    // Some computation
    val = static_cast<int16_t>(val + 42);
    TS_ASSERT_EQUALS(val.m_bytes[0], 42U);
    TS_ASSERT_EQUALS(val.m_bytes[1], 0U);

    val = static_cast<int16_t>(val * 10);
    TS_ASSERT_EQUALS(val.m_bytes[0], 164U);
    TS_ASSERT_EQUALS(val.m_bytes[1], 1U);

    int16_t v = val;
    TS_ASSERT_EQUALS(v, 420);
}

/** Test different signed types. */
void
TestBitsValue::testSignedTypes()
{
    // Int8
    {
        afl::bits::Value<afl::bits::Int8> val;
        TS_ASSERT_EQUALS(sizeof(val), 1U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        val = 0x12;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0x12U);
        val = -3;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0xFDU);
    }

    // Int16LE
    {
        afl::bits::Value<afl::bits::Int16LE> val;
        TS_ASSERT_EQUALS(sizeof(val), 2U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0U);
        val = 0x1234;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0x34U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0x12U);
        val = -3;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0xFDU);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0xFFU);
    }

    // Int32LE
    {
        afl::bits::Value<afl::bits::Int32LE> val;
        TS_ASSERT_EQUALS(sizeof(val), 4U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0U);
        val = 0x12345678;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0x78U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0x56U);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0x34U);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0x12U);
        val = -3;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0xFDU);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0xFFU);
    }

    // Int64LE
    {
        afl::bits::Value<afl::bits::Int64LE> val;
        TS_ASSERT_EQUALS(sizeof(val), 8U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[4], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[5], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[6], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[7], 0U);
        val = 0x123456789ABCDEF0LL;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0xF0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0xDEU);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0xBCU);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0x9AU);
        TS_ASSERT_EQUALS(val.m_bytes[4], 0x78U);
        TS_ASSERT_EQUALS(val.m_bytes[5], 0x56U);
        TS_ASSERT_EQUALS(val.m_bytes[6], 0x34U);
        TS_ASSERT_EQUALS(val.m_bytes[7], 0x12U);
        val = -3;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0xFDU);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[4], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[5], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[6], 0xFFU);
        TS_ASSERT_EQUALS(val.m_bytes[7], 0xFFU);
    }
}

/** Test different unsigned types. */
void
TestBitsValue::testUnsignedTypes()
{
    // UInt8
    {
        afl::bits::Value<afl::bits::UInt8> val;
        TS_ASSERT_EQUALS(sizeof(val), 1U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        val = 0x12;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0x12U);
    }

    // UInt16LE
    {
        afl::bits::Value<afl::bits::UInt16LE> val;
        TS_ASSERT_EQUALS(sizeof(val), 2U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0U);
        val = 0x1234;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0x34U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0x12U);
    }

    // UInt32LE
    {
        afl::bits::Value<afl::bits::UInt32LE> val;
        TS_ASSERT_EQUALS(sizeof(val), 4U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0U);
        val = 0x12345678;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0x78U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0x56U);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0x34U);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0x12U);
    }

    // UInt64LE
    {
        afl::bits::Value<afl::bits::UInt64LE> val;
        TS_ASSERT_EQUALS(sizeof(val), 8U);
        val = 0;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[4], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[5], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[6], 0U);
        TS_ASSERT_EQUALS(val.m_bytes[7], 0U);
        val = 0x123456789ABCDEF0ULL;
        TS_ASSERT_EQUALS(val.m_bytes[0], 0xF0U);
        TS_ASSERT_EQUALS(val.m_bytes[1], 0xDEU);
        TS_ASSERT_EQUALS(val.m_bytes[2], 0xBCU);
        TS_ASSERT_EQUALS(val.m_bytes[3], 0x9AU);
        TS_ASSERT_EQUALS(val.m_bytes[4], 0x78U);
        TS_ASSERT_EQUALS(val.m_bytes[5], 0x56U);
        TS_ASSERT_EQUALS(val.m_bytes[6], 0x34U);
        TS_ASSERT_EQUALS(val.m_bytes[7], 0x12U);
    }
}

/** Test structure layout. */
void
TestBitsValue::testStructure()
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
    TS_ASSERT_EQUALS(sizeof(SomeStruct), 11U);
}
