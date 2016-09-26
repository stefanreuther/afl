/**
  *  \file afl/async/operation.hpp
  *  \brief Class afl::async::Operation
  */
#ifndef AFL_AFL_ASYNC_OPERATION_HPP
#define AFL_AFL_ASYNC_OPERATION_HPP

#include "afl/base/uncopyable.hpp"

namespace afl { namespace async {

    class Controller;
    class Cancelable;
    class Notifier;

    /** Asynchronous operation.
        An object of this class identifies an asynchronous operation.
        Derived classes keep data required for executing the operation.

        \todo this is incomplete */
    class Operation : public afl::base::Uncopyable {
     public:
        /** Constructor. */
        Operation();

        /** Destructor. */
        virtual ~Operation();

        /** Set controller. Used by asynchronous operation implementations.
            \param pController controller */
        void setController(Controller* pController)
            { m_pController = pController; }

        /** Get associated controller. Used by asynchronous operation implementations.
            \return the controller */
        Controller* getController() const
            { return m_pController; }

        /** Set notifier.
            \param notifier Notifier */
        void setNotifier(Notifier& notifier)
            { m_pNotifier = &notifier; }

        /** Get notifier.
            \return the notifier */
        Notifier& getNotifier() const
            { return *m_pNotifier; }

     private:
        Controller* m_pController;
        Notifier* m_pNotifier;
    };

} }

#endif
