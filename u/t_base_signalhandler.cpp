/**
  *  \file u/t_base_signalhandler.cpp
  *  \brief Test for afl::base::SignalHandler
  */

#include "afl/base/signalhandler.hpp"

#include "u/t_base.hpp"


/** Simple test. */
void
TestBaseSignalHandler::testIt()
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
        afl::base::SignalHandler* hs[LIMIT];
        for (int j = 0; j < LIMIT; ++j) {
            list = hs[j] = new MySignalHandler(&list);
        }

        // Delete one.
        delete hs[i];

        // Check that list is still iterable without problems.
        int count = 0;
        afl::base::SignalHandler* p = list;
        while (count < 20 && p != 0) {
            p = p->next();
            ++count;
        }

        // Must have found 9 elements.
        TS_ASSERT_EQUALS(count, LIMIT-1);

        // Clean up
        for (int j = 0; j < LIMIT; ++j) {
            if (j != i) {
                delete hs[j];
            }
        }
    }
}
