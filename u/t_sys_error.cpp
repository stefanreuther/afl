/**
  *  \file u/t_sys_error.cpp
  *  \brief Test for afl::sys::Error
  */

#include "afl/sys/error.hpp"

#include "u/t_sys.hpp"

/** Simple test. */
void
TestSysError::testIt()
{
    afl::sys::Error err;
    TS_ASSERT(!err);
    TS_ASSERT_EQUALS(err.getNumber(), 0);

    afl::sys::Error err0;
    TS_ASSERT(!err0);
    TS_ASSERT_EQUALS(err0.getNumber(), 0);
    TS_ASSERT_EQUALS(err, err0);

    afl::sys::Error err1(1);
    afl::sys::Error err2(2);
    TS_ASSERT(err1);
    TS_ASSERT(err2);
    TS_ASSERT_EQUALS(err1.getNumber(), 1);
    TS_ASSERT_EQUALS(err2.getNumber(), 2);

    TS_ASSERT( (err0 != err1));
    TS_ASSERT(!(err0 == err1));
    TS_ASSERT( (err0 <= err1));
    TS_ASSERT(!(err0 >= err1));
    TS_ASSERT( (err0 <  err1));
    TS_ASSERT(!(err0 >  err1));

    TS_ASSERT( (err1 != err2));
    TS_ASSERT(!(err1 == err2));
    TS_ASSERT( (err1 <= err2));
    TS_ASSERT(!(err1 >= err2));
    TS_ASSERT( (err1 <  err2));
    TS_ASSERT(!(err1 >  err2));

    TS_ASSERT( (err2 != err1));
    TS_ASSERT(!(err2 == err1));
    TS_ASSERT(!(err2 <= err1));
    TS_ASSERT( (err2 >= err1));
    TS_ASSERT(!(err2 <  err1));
    TS_ASSERT( (err2 >  err1));

    TS_ASSERT(!(err1 != err1));
    TS_ASSERT( (err1 == err1));
    TS_ASSERT( (err1 <= err1));
    TS_ASSERT( (err1 >= err1));
    TS_ASSERT(!(err1 <  err1));
    TS_ASSERT(!(err1 >  err1));

    err = err2;
    TS_ASSERT_EQUALS(err.getNumber(), 2);
}
