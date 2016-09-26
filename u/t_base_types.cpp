/**
  *  \file u/t_base_types.cpp
  *  \brief Test for afl/base/types.hpp
  */

#include "afl/base/types.hpp"
#include "u/t_base.hpp"

/** Test 8-bit types.
    Tests that the types.hpp file provides the types we expect with the properties we expect. */
void
TestBaseTypes::test8()
{
    {
        int8_t i8 = 0;
        TS_ASSERT_EQUALS(i8, 0);
    }
    {
        int8_t i8 = -128;
        TS_ASSERT_EQUALS(i8, -128);
    }
    {
        int8_t i8 = +127;
        TS_ASSERT_EQUALS(i8, +127);
    }
    {
        uint8_t u8 = 0;
        TS_ASSERT_EQUALS(u8, 0U);
    }
    {
        uint8_t u8 = 255;
        TS_ASSERT_EQUALS(u8, 255U);
    }
}

/** Test 16-bit types. */
void
TestBaseTypes::test16()
{
    {
        int16_t i16 = 0;
        TS_ASSERT_EQUALS(i16, 0);
    }
    {
        int16_t i16 = -32767-1;
        TS_ASSERT_EQUALS(i16, -32767-1);
    }
    {
        int16_t i16 = +32767;
        TS_ASSERT_EQUALS(i16, +32767);
    }
    {
        uint16_t u16 = 0;
        TS_ASSERT_EQUALS(u16, 0U);
    }
    {
        uint16_t u16 = 65535;
        TS_ASSERT_EQUALS(u16, 65535U);
    }
}

/** Test 32-bit types. */
void
TestBaseTypes::test32()
{
    {
        int32_t i32 = 0;
        TS_ASSERT_EQUALS(i32, 0);
    }
    {
        int32_t i32 = -2147483647-1;
        TS_ASSERT_EQUALS(i32, -2147483647-1);
    }
    {
        int32_t i32 = +2147483647;
        TS_ASSERT_EQUALS(i32, +2147483647);
    }
    {
        uint32_t u32 = 0;
        TS_ASSERT_EQUALS(u32, 0U);
    }
    {
        uint32_t u32 = 0xFFFFFFFFU;
        TS_ASSERT_EQUALS(u32, 0xFFFFFFFFU);
    }
}

/** Test 64-bit types. */
void
TestBaseTypes::test64()
{
    {
        int64_t i64 = 0;
        TS_ASSERT_EQUALS(i64, 0);
    }
    {
        int64_t i64 = -9223372036854775807LL-1;
        TS_ASSERT_EQUALS(i64, -9223372036854775807LL-1);
    }
    {
        int64_t i64 = +9223372036854775807LL;
        TS_ASSERT_EQUALS(i64, +9223372036854775807LL);
    }
    {
        uint64_t u64 = 0;
        TS_ASSERT_EQUALS(u64, 0U);
    }
    {
        uint64_t u64 = 0xFFFFFFFFFFFFFFFFULL;
        TS_ASSERT_EQUALS(u64, 0xFFFFFFFFFFFFFFFFULL);
    }
}

/** Test size types. */
void
TestBaseTypes::testSize()
{
    {
        size_t sz = 0;
        TS_ASSERT_EQUALS(sz, 0U);
    }
    {
        size_t sz = sizeof(int);
        TS_ASSERT(sz > 0);
    }
    {
        size_t sz = static_cast<size_t>(-1);
        TS_ASSERT(sz > 0);
    }
}
