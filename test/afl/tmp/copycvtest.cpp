/**
  *  \file test/afl/tmp/copycvtest.cpp
  *  \brief Test for afl::tmp::CopyCV
  */

#include "afl/tmp/copycv.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.tmp.CopyCV:plain", a)
{
    int* pi1 = 0;
    afl::tmp::CopyCV<long, int>::Type* pi2 = 0;
    pi1 = pi2;
    pi2 = pi1;
    a.check("", pi1 == pi2);
}

AFL_TEST("afl.tmp.CopyCV:const", a)
{
    const int* pci1 = 0;
    afl::tmp::CopyCV<const long, int>::Type* pci2 = 0;
    pci1 = pci2;
    pci2 = pci1;
    a.check("", pci1 == pci2);
}

AFL_TEST("afl.tmp.CopyCV:volatile", a)
{
    volatile int* pvi1 = 0;
    afl::tmp::CopyCV<volatile long, int>::Type* pvi2 = 0;
    pvi1 = pvi2;
    pvi2 = pvi1;
    a.check("", pvi1 == pvi2);
}

AFL_TEST("afl.tmp.CopyCV:const-volatile", a)
{
    const volatile int* pcvi1 = 0;
    afl::tmp::CopyCV<const volatile long, int>::Type* pcvi2 = 0;
    pcvi1 = pcvi2;
    pcvi2 = pcvi1;
    a.check("", pcvi1 == pcvi2);
}
