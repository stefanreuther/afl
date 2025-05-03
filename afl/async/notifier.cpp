/**
  *  \file afl/async/notifier.cpp
  *  \brief Class afl::async::Notifier
  */

#include "afl/async/notifier.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/controller.hpp"

namespace {
    /*
     *  Default Notifier
     */
    class DefaultNotifier : public afl::async::Notifier {
     public:
        virtual void notify(afl::async::Operation& op)
            {
                op.getController()->post(op);
            }
    };
}

afl::async::Notifier::~Notifier()
{ }

afl::async::Notifier&
afl::async::Notifier::getDefaultInstance()
{
    static DefaultNotifier instance;
    return instance;
}
