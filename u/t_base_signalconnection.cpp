/**
  *  \file u/t_base_signalconnection.cpp
  *  \brief Test for afl::base::SignalConnection
  */

#include "afl/base/signalconnection.hpp"

#include "u/t_base.hpp"

/** Simple tests. */
void
TestBaseSignalConnection::testIt()
{
    // Must re-export constructor because it is protected.
    class MySignalHandler : public afl::base::SignalHandler {
     public:
        MySignalHandler(afl::base::SignalHandler** pPrev)
            : afl::base::SignalHandler(pPrev)
            { }
    };
    const int LIMIT = 10;

    for (int i = 0; i < LIMIT; ++i) {
        // Make LIMIT signal handlers.
        afl::base::SignalHandler* list = 0;
        afl::base::SignalConnection hs[LIMIT];
        for (int j = 0; j < LIMIT; ++j) {
            hs[j] = list = new MySignalHandler(&list);
        }

        // Delete one.
        hs[i].disconnect();

        // Check that list is still iterable without problems.
        int count = 0;
        afl::base::SignalHandler* p = list;
        while (count < 20 && p != 0) {
            p = p->next();
            ++count;
        }

        // Must have found 9 elements.
        TS_ASSERT_EQUALS(count, LIMIT-1);

        // Clean up. This will double-disconnect one, which must be a supported use-case.
        for (int j = 0; j < LIMIT; ++j) {
            hs[j].disconnect();
        }
    }
}
