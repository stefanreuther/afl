/**
  *  \file afl/async/cancelable.hpp
  *  \brief Interface afl::async::Cancelable
  */
#ifndef AFL_AFL_ASYNC_CANCELABLE_HPP
#define AFL_AFL_ASYNC_CANCELABLE_HPP

#include "afl/base/deletable.hpp"

namespace afl { namespace async {

    class Operation;
    class Controller;

    /** Cancelable object.
        This is the base class for objects that support asynchronous operations.
        \todo Reconsider this class; probably a better name? */
    class Cancelable : public afl::base::Deletable {
     public:
        /** Cancel an operation.
            If the operation has been posted on this object, this controller, cancels it.
            (If the operation already completed, avoids that it is reported by the Controller.)

            Implementations must
            - cancel the actual operation
            - call ctl.revertPost(op) to avoid that it is reported by the Controller.

            This function must, like all functions that access a Controller,
            be called from the thread that owns the Controller.

            \param ctl Controller
            \param op Operation */
        virtual void cancel(Controller& ctl, Operation& op) = 0;
    };

} }

#endif
