/**
  *  \file afl/base/stoppable.hpp
  *  \brief Interface afl::base::Stoppable
  */
#ifndef AFL_AFL_BASE_STOPPABLE_HPP
#define AFL_AFL_BASE_STOPPABLE_HPP

#include "afl/base/runnable.hpp"

namespace afl { namespace base {

    /** Interface for some infinitely-running, stoppable action. */
    class Stoppable : public Runnable {
     public:
        /** Cause run() to exit.
            Can be called from another thread as run().

            This function can return before run() has exited.
            Typically, stop() signals run() to stop, causing a thread to exit.
            You can then use join() on that thread.

            stop() must handle being called multiple times. */
        virtual void stop() = 0;
    };

} }

#endif
