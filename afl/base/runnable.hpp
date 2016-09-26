/**
  *  \file afl/base/runnable.hpp
  *  \brief Interface afl::base::Runnable
  */
#ifndef AFL_AFL_BASE_RUNNABLE_HPP
#define AFL_AFL_BASE_RUNNABLE_HPP

#include "afl/base/deletable.hpp"

namespace afl { namespace base {

    /** Interface for some runnable action. */
    class Runnable : public Deletable {
     public:
        /** Run this action. */
        virtual void run() = 0;
    };

} }

#endif
