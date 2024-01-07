/**
  *  \file test/afl/sys/errortest.cpp
  *  \brief Test for afl::sys::Error
  */

#include "afl/sys/error.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.sys.Error", a)
{
    afl::sys::Error err;
    a.check("01. op!", !err);
    a.checkEqual("02. getNumber", err.getNumber(), 0);

    afl::sys::Error err0;
    a.check("11. op!", !err0);
    a.checkEqual("12. getNumber", err0.getNumber(), 0);
    a.check("13. op==", err == err0);

    afl::sys::Error err1(1);
    afl::sys::Error err2(2);
    a.check("21. bool", err1);
    a.check("22. bool", err2);
    a.checkEqual("23. getNumber", err1.getNumber(), 1);
    a.checkEqual("24. getNumber", err2.getNumber(), 2);

    a.check("31. ne",  (err0 != err1));
    a.check("32. eq", !(err0 == err1));
    a.check("33. le",  (err0 <= err1));
    a.check("34. ge", !(err0 >= err1));
    a.check("35. lt",  (err0 <  err1));
    a.check("36. gt", !(err0 >  err1));

    a.check("41. ne",  (err1 != err2));
    a.check("42. eq", !(err1 == err2));
    a.check("43. le",  (err1 <= err2));
    a.check("44. ge", !(err1 >= err2));
    a.check("45. lt",  (err1 <  err2));
    a.check("46. gt", !(err1 >  err2));

    a.check("51. ne",  (err2 != err1));
    a.check("52. eq", !(err2 == err1));
    a.check("53. le", !(err2 <= err1));
    a.check("54. ge",  (err2 >= err1));
    a.check("55. lt", !(err2 <  err1));
    a.check("56. gt",  (err2 >  err1));

    a.check("61. ne", !(err1 != err1));
    a.check("62. eq",  (err1 == err1));
    a.check("63. le",  (err1 <= err1));
    a.check("64. ge",  (err1 >= err1));
    a.check("65. lt", !(err1 <  err1));
    a.check("66. gt", !(err1 >  err1));

    err = err2;
    a.checkEqual("71. getNumber after assign", err.getNumber(), 2);
}
