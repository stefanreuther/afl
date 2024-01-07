/**
  *  \file test/afl/base/typestest.cpp
  *  \brief Test for afl::base::Types
  */

#include "afl/base/types.hpp"
#include "afl/test/testrunner.hpp"

/** Test 8-bit types.
    Tests that the types.hpp file provides the types we expect with the properties we expect. */
AFL_TEST("afl.base.Types:i8:zero", a) {
    int8_t i8 = 0;
    a.checkEqual("", i8, 0);
}

AFL_TEST("afl.base.Types:i8:min", a) {
    int8_t i8 = -128;
    a.checkEqual("", i8, -128);
}

AFL_TEST("afl.base.Types:i8:max", a) {
    int8_t i8 = +127;
    a.checkEqual("", i8, +127);
}

AFL_TEST("afl.base.Types:u8:min", a) {
    uint8_t u8 = 0;
    a.checkEqual("", u8, 0U);
}

AFL_TEST("afl.base.Types:u8:max", a) {
    uint8_t u8 = 255;
    a.checkEqual("", u8, 255U);
}

/** Test 16-bit types. */
AFL_TEST("afl.base.Types:i16:zero", a) {
    int16_t i16 = 0;
    a.checkEqual("", i16, 0);
}

AFL_TEST("afl.base.Types:i16:min", a) {
    int16_t i16 = -32767-1;
    a.checkEqual("", i16, -32767-1);
}

AFL_TEST("afl.base.Types:i16:max", a) {
    int16_t i16 = +32767;
    a.checkEqual("", i16, +32767);
}

AFL_TEST("afl.base.Types:u16:min", a) {
    uint16_t u16 = 0;
    a.checkEqual("", u16, 0U);
}

AFL_TEST("afl.base.Types:u16:max", a) {
    uint16_t u16 = 65535;
    a.checkEqual("", u16, 65535U);
}

/** Test 32-bit types. */
AFL_TEST("afl.base.Types:i32:zero", a) {
    int32_t i32 = 0;
    a.checkEqual("", i32, 0);
}

AFL_TEST("afl.base.Types:i32:min", a) {
    int32_t i32 = -2147483647-1;
    a.checkEqual("", i32, -2147483647-1);
}

AFL_TEST("afl.base.Types:i32:max", a) {
    int32_t i32 = +2147483647;
    a.checkEqual("", i32, +2147483647);
}

AFL_TEST("afl.base.Types:u32:min", a) {
    uint32_t u32 = 0;
    a.checkEqual("", u32, 0U);
}

AFL_TEST("afl.base.Types:u32:max", a) {
    uint32_t u32 = 0xFFFFFFFFU;
    a.checkEqual("", u32, 0xFFFFFFFFU);
}


/** Test 64-bit types.
    Using 'check(...==)' here instead of 'checkEqual'
    because the literals may end up with a different type
    that Format() cannot print. */
AFL_TEST("afl.base.Types:i64:zero", a) {
    int64_t i64 = 0;
    a.check("", i64 == 0);
}

AFL_TEST("afl.base.Types:i64:min", a) {
    int64_t i64 = -9223372036854775807LL-1;
    a.check("", i64 == -9223372036854775807LL-1);
}

AFL_TEST("afl.base.Types:i64:max", a) {
    int64_t i64 = +9223372036854775807LL;
    a.check("", i64 == +9223372036854775807LL);
}

AFL_TEST("afl.base.Types:u64:min", a) {
    uint64_t u64 = 0;
    a.check("", u64 == 0U);
}

AFL_TEST("afl.base.Types:u64:max", a) {
    uint64_t u64 = 0xFFFFFFFFFFFFFFFFULL;
    a.check("", u64 == 0xFFFFFFFFFFFFFFFFULL);
}

/** Test size types. */
AFL_TEST("afl.base.Types:size:zero", a) {
    size_t sz = 0;
    a.check("", sz == 0U);
}

AFL_TEST("afl.base.Types:size:int", a) {
    size_t sz = sizeof(int);
    a.check("", sz > 0);
}

AFL_TEST("afl.base.Types:size:max", a) {
    size_t sz = static_cast<size_t>(-1);
    a.check("", sz > 0);
}
