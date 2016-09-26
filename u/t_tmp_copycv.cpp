/**
  *  \file u/t_tmp_copycv.cpp
  *  \brief Test for afl::tmp::CopyCV
  */

#include "afl/tmp/copycv.hpp"

#include "u/t_tmp.hpp"

/** Simple test. */
void
TestTmpCopyCV::testIt()
{
    {
        int* pi1 = 0;
        afl::tmp::CopyCV<long, int>::Type* pi2 = 0;
        pi1 = pi2;
        pi2 = pi1;
        TS_ASSERT_EQUALS(pi1, pi2);
    }
    {
        const int* pci1 = 0;
        afl::tmp::CopyCV<const long, int>::Type* pci2 = 0;
        pci1 = pci2;
        pci2 = pci1;
        TS_ASSERT_EQUALS(pci1, pci2);
    }
    {
        volatile int* pvi1 = 0;
        afl::tmp::CopyCV<volatile long, int>::Type* pvi2 = 0;
        pvi1 = pvi2;
        pvi2 = pvi1;
        TS_ASSERT_EQUALS(pvi1, pvi2);
    }
    {
        const volatile int* pcvi1 = 0;
        afl::tmp::CopyCV<const volatile long, int>::Type* pcvi2 = 0;
        pcvi1 = pcvi2;
        pcvi2 = pcvi1;
        TS_ASSERT_EQUALS(pcvi1, pcvi2);
    }
}
