/**
  *  \file arch/posix/posixinterrupt.cpp
  *  \brief Class arch::posix::PosixInterrupt
  */

#if TARGET_OS_POSIX
#include <sys/fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "arch/posix/posixinterrupt.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/sys/mutexguard.hpp"
#include "arch/posix/posixfiledescriptor.hpp"

using afl::async::InterruptOperation;

namespace {
    /** Global instance pointer.
        The user (which is afl::async::Interrupt) will create only one instance.
        However, instead of going through their pointer, we use our own which is typesafe and works in signals. */
    arch::posix::PosixInterrupt* g_pInstance;

    /** Signal handler. */
    extern "C" void signalHandler(int signo)
    {
        if (arch::posix::PosixInterrupt* p = g_pInstance) {
            p->handleSignal(signo);
        }
    }
}


class arch::posix::PosixInterrupt::Instance : public afl::async::Notifier {
 public:
    struct WaitOperation : public afl::async::ReceiveOperation {
        uint8_t m_byte[1];
        afl::async::Controller* m_pController;
    };

    Instance(afl::async::InterruptOperation::Kind k, int signalNumber, PosixInterrupt& parent);
    ~Instance();

    /** Wake this Instance, from signal handler. */
    void wakeFromSignal();

    /** Wait on a controller.
        Uses the given controller to listen for a signalisation. */
    void wait(afl::async::Controller& ctl);

    /** Cancel wait on a controller. */
    void cancel(afl::async::Controller& ctl);

    // Notifier:
    void notify(afl::async::Operation& op);
    void notifyDirect(afl::async::Operation& op);

 public:
    const afl::async::InterruptOperation::Kind m_kind;
    const int m_signalNumber;
    PosixInterrupt& m_parent;

    enum { Read, Write };
    int m_pipe[2];

    std::list<WaitOperation*> m_pendingWaits;

    afl::sys::Mutex m_mutex;
    std::auto_ptr<arch::posix::PosixFileDescriptor> m_readFD;
    struct sigaction m_oldAction;
};



arch::posix::PosixInterrupt::Instance::Instance(afl::async::InterruptOperation::Kind k, int signalNumber, PosixInterrupt& parent)
    : m_kind(k),
      m_signalNumber(signalNumber),
      m_parent(parent),
      m_pendingWaits(),
      m_mutex(),
      m_readFD()
{
    // Creata a pipe
    ::pipe(m_pipe);
    ::fcntl(m_pipe[Read],  F_SETFL, O_NONBLOCK);
    ::fcntl(m_pipe[Write], F_SETFL, O_NONBLOCK);
    ::fcntl(m_pipe[Read],  F_SETFD, FD_CLOEXEC);
    ::fcntl(m_pipe[Write], F_SETFD, FD_CLOEXEC);

    // The read end is managed as a PosixFileDescriptor (which also owns the file descriptor)
    m_readFD.reset(new PosixFileDescriptor(m_pipe[Read]));

    // Hook the signal and save the old handler
    struct sigaction newAction;
    newAction.sa_handler = signalHandler;
    ::sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = SA_RESTART;
    ::sigaction(m_signalNumber, &newAction, &m_oldAction);
}

arch::posix::PosixInterrupt::Instance::~Instance()
{
    // Ditch all wait operations (there shouldn't be any)
    {
        afl::sys::MutexGuard g(m_mutex);
        while (!m_pendingWaits.empty()) {
            WaitOperation* p = m_pendingWaits.front();
            m_pendingWaits.pop_front();
            m_readFD->cancel(*p->m_pController, *p);
            delete p;
        }
    }

    // Unhook the signal
    ::sigaction(m_signalNumber, &m_oldAction, 0);

    // Close the pipe. The read-end is closed by PosixFileDescriptor.
    ::close(m_pipe[Write]);
}

void
arch::posix::PosixInterrupt::Instance::wakeFromSignal()
{
    ::write(m_pipe[Write], "", 1);
}

void
arch::posix::PosixInterrupt::Instance::wait(afl::async::Controller& ctl)
{
    WaitOperation* p = new WaitOperation();
    p->setData(p->m_byte);
    p->setNotifier(*this);
    p->m_pController = &ctl;
    {
        afl::sys::MutexGuard g(m_mutex);
        m_pendingWaits.push_back(p);
    }
    m_readFD->receiveAsync(ctl, *p);
}

void
arch::posix::PosixInterrupt::Instance::cancel(afl::async::Controller& ctl)
{
    afl::sys::MutexGuard g(m_mutex);
    for (std::list<WaitOperation*>::iterator it = m_pendingWaits.begin(), e = m_pendingWaits.end(); it != e; ++it) {
        WaitOperation* p = *it;
        if (p->m_pController == &ctl) {
            m_pendingWaits.erase(it);
            m_readFD->cancel(ctl, *p);
            delete p;
            break;
        }
    }
}

void
arch::posix::PosixInterrupt::Instance::notify(afl::async::Operation& /*op*/)
{
    m_parent.notifyInterrupt(m_kind);
}

void
arch::posix::PosixInterrupt::Instance::notifyDirect(afl::async::Operation& op)
{
    notify(op);
}

/***************************** PosixInterrupt ****************************/

arch::posix::PosixInterrupt::PosixInterrupt()
    : m_int(), m_hup(), m_term()
{
    g_pInstance = this;
}

arch::posix::PosixInterrupt::~PosixInterrupt()
{
    // Reset and unhook signals
    m_int.reset();
    m_hup.reset();
    m_term.reset();

    // Unregister
    g_pInstance = 0;
}

afl::async::InterruptOperation::Kinds_t
arch::posix::PosixInterrupt::wait(afl::async::Controller& ctl, afl::async::InterruptOperation::Kinds_t kinds, afl::sys::Timeout_t timeout)
{
    afl::async::InterruptOperation op(kinds);
    waitAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return afl::async::InterruptOperation::Kinds_t();
    } else {
        return op.getReceived();
    }
}

void
arch::posix::PosixInterrupt::waitAsync(afl::async::Controller& ctl, afl::async::InterruptOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    waitOnce(m_int,  InterruptOperation::Break,     SIGINT,  ctl, op);
    waitOnce(m_hup,  InterruptOperation::Hangup,    SIGHUP,  ctl, op);
    waitOnce(m_term, InterruptOperation::Terminate, SIGTERM, ctl, op);
    op.setController(&ctl);
    m_pending.push_back(&op);
}

void
arch::posix::PosixInterrupt::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    std::list<afl::async::InterruptOperation*>::iterator it = std::find(m_pending.begin(), m_pending.end(), &op);
    if (it != m_pending.end()) {
        // Found the request. Remove it from our list and cancel the waits.
        afl::async::InterruptOperation& found = **it;
        m_pending.erase(it);
        cancelOnce(m_int,  InterruptOperation::Break,     found);
        cancelOnce(m_hup,  InterruptOperation::Hangup,    found);
        cancelOnce(m_term, InterruptOperation::Terminate, found);
    } else {
        // Not found. It might still be in the controller.
        ctl.revertPost(op);
    }
}

void
arch::posix::PosixInterrupt::notifyInterrupt(afl::async::InterruptOperation::Kind k)
{
    afl::sys::MutexGuard g(m_mutex);
    std::list<afl::async::InterruptOperation*>::iterator it = m_pending.begin();
    while (it != m_pending.end()) {
        afl::async::InterruptOperation* op = *it;
        if (op->getRequested().contains(k)) {
            // This request wants the interrupt. Remove from pending list, ...
            it = m_pending.erase(it);

            // ...cancel waits...
            cancelOnce(m_int,  InterruptOperation::Break,     *op);
            cancelOnce(m_hup,  InterruptOperation::Hangup,    *op);
            cancelOnce(m_term, InterruptOperation::Terminate, *op);

            // ...and notify it.
            op->setReceived(afl::async::InterruptOperation::Kinds_t(k));
            op->getNotifier().notify(*op);
        } else {
            // Skip it
            ++it;
        }
    }
}

void
arch::posix::PosixInterrupt::handleSignal(int n)
{
    switch (n) {
     case SIGINT:
        if (Instance* p = m_int.get()) {
            p->wakeFromSignal();
        }
        break;
     case SIGHUP:
        if (Instance* p = m_hup.get()) {
            p->wakeFromSignal();
        }
        break;
     case SIGTERM:
        if (Instance* p = m_term.get()) {
            p->wakeFromSignal();
        }
        break;
    }
}

inline void
arch::posix::PosixInterrupt::cancelOnce(std::auto_ptr<Instance>& p, InterruptOperation_t::Kind k, InterruptOperation_t& op)
{
    if (op.getRequested().contains(k)) {
        if (p.get() != 0) {
            p->cancel(*op.getController());
        }
    }
}

inline void
arch::posix::PosixInterrupt::waitOnce(std::auto_ptr<Instance>& p, InterruptOperation_t::Kind k, int signalNumber, afl::async::Controller& ctl, InterruptOperation_t& op)
{
    if (op.getRequested().contains(k)) {
        if (p.get() == 0) {
            p.reset(new Instance(k, signalNumber, *this));
        }
        p->wait(ctl);
    }
}
#else
int g_variableToMakePosixInterruptObjectFileNotEmpty;
#endif
