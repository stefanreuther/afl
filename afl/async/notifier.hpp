/**
  *  \file afl/async/notifier.hpp
  *  \brief Class afl::async::Notifier
  */
#ifndef AFL_AFL_ASYNC_NOTIFIER_HPP
#define AFL_AFL_ASYNC_NOTIFIER_HPP

namespace afl { namespace async {

    class Operation;

    /** Notification strategy for asynchronous operations.
        The default strategy is to post the result on a Controller.
        Sometimes (especially when implementing custom async objects)
        we do not want the results posted directly, but we want to
        intercept or modify the requests inbetween.
        In this case, a custom notifier can be used.

        Each Operation has an associated Notifier. If it completes,
        it calls notify() on it. */
    class Notifier {
     public:
        /** Virtual destructor. */
        virtual ~Notifier();

        /** Post notification.
            This method corresponds to Controller::post,
            and can be called from any thread.
            \param op Operation to notify */
        virtual void notify(Operation& op) = 0;

        /** Get default instance.
            The default notifier instance calls post()/postDirect()
            on the operation's controller. */
        static Notifier& getDefaultInstance();
    };

} }

#endif
