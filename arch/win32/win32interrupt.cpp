/**
  *  \file arch/win32/win32interrupt.cpp
  *  \brief Class arch::win32::Win32Interrupt
  */

#ifdef TARGET_OS_WIN32
#include <algorithm>
#include "arch/win32/win32interrupt.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/sys/error.hpp"
#include "afl/sys/mutexguard.hpp"
#include "arch/controller.hpp"

using afl::async::InterruptOperation;

namespace {
    arch::win32::Win32Interrupt* s_pInstance;

    BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
    {
        BOOL result = FALSE;
        if (s_pInstance != 0) {
            switch (dwCtrlType) {
            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
                if (s_pInstance->handleInterrupt(InterruptOperation::Break)) {
                    result = TRUE;
                }
                break;

            case CTRL_LOGOFF_EVENT:
            case CTRL_SHUTDOWN_EVENT:
                if (s_pInstance->handleInterrupt(InterruptOperation::Hangup)) {
                    result = TRUE;
                }
                break;

            case CTRL_CLOSE_EVENT:
                if (s_pInstance->handleInterrupt(InterruptOperation::Terminate)) {
                    result = TRUE;
                }
                break;
            }
        }
        return result;
    }
}

arch::win32::Win32Interrupt::Win32Interrupt()
    : m_pending(),
      m_mutex(),
      m_event(CreateEvent(0, FALSE, FALSE, 0))
{
    // Refuse errors
    if (m_event == NULL) {
        throw afl::except::SystemException(afl::sys::Error::current(), "<CreateEvent>");
    }

    // Verify that array indexes are in range
    static_assert(InterruptOperation_t::Break >= 0 && InterruptOperation_t::Break < N, "Break");
    static_assert(InterruptOperation_t::Hangup >= 0 && InterruptOperation_t::Hangup < N, "Hangup");
    static_assert(InterruptOperation_t::Terminate >= 0 && InterruptOperation_t::Terminate < N, "Terminate");

    // Initialize arrays
    for (size_t i = 0; i < N; ++i) {
        m_attached[i] = 0;
        m_signalled[i] = 0;
    }

    // Set up
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);
    s_pInstance = this;
}

/** Destructor. */
arch::win32::Win32Interrupt::~Win32Interrupt()
{
    SetConsoleCtrlHandler(HandlerRoutine, FALSE);
    s_pInstance = 0;
    CloseHandle(m_event);
}

// Interrupt:
afl::async::InterruptOperation::Kinds_t
arch::win32::Win32Interrupt::wait(afl::async::Controller& ctl, afl::async::InterruptOperation::Kinds_t kinds, afl::sys::Timeout_t timeout)
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
arch::win32::Win32Interrupt::waitAsync(afl::async::Controller& ctl, afl::async::InterruptOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    setupOnce(op, InterruptOperation::Break);
    setupOnce(op, InterruptOperation::Hangup);
    setupOnce(op, InterruptOperation::Terminate);

    // Save user operation
    op.setController(&ctl);
    m_pending.push_back(&op);

    // Add wait operation
    ctl.getImplementation().addRequest(*this, m_event);
}

void
arch::win32::Win32Interrupt::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    std::list<InterruptOperation_t*>::iterator it = std::find(m_pending.begin(), m_pending.end(), &op);
    if (it != m_pending.end()) {
        // It's still waiting. Remove it and cancel the wait.
        m_pending.erase(it);
        ctl.getImplementation().removeRequest(*this, m_event);
    } else {
        // It might already be signaled; remove that
        ctl.revertPost(op);
    }
}

bool
arch::win32::Win32Interrupt::handleInterrupt(InterruptOperation_t::Kind k)
{
    bool result;
    if (m_attached[k] != 0) {
        // We are attached to this event, handle it.
        InterlockedExchangeAdd(&m_signalled[k], 1);
        SetEvent(m_event);
        result = true;
    } else {
        // Not attached; ignore that event.
        result = false;
    }
    return result;
}

bool
arch::win32::Win32Interrupt::handleWaitReady()
{
    // Build set
    InterruptOperation_t::Kinds_t ints;
    if (InterlockedExchangeAdd(&m_signalled[InterruptOperation_t::Break], 0)) {
        ints += InterruptOperation_t::Break;
    }
    if (InterlockedExchangeAdd(&m_signalled[InterruptOperation_t::Hangup], 0)) {
        ints += InterruptOperation_t::Hangup;
    }
    if (InterlockedExchangeAdd(&m_signalled[InterruptOperation_t::Terminate], 0)) {
        ints += InterruptOperation_t::Terminate;
    }

    // Signal everything
    afl::sys::MutexGuard g(m_mutex);
    std::list<InterruptOperation_t*>::iterator it = m_pending.begin();
    while (it != m_pending.end()) {
        InterruptOperation_t* op = *it;
        InterruptOperation_t::Kinds_t match = op->getRequested() & ints;
        if (!match.empty()) {
            // This request wants the interrupt. Remove from pending list, ...
            it = m_pending.erase(it);

            // ...cancel OS waits...
            op->getController()->getImplementation().removeRequest(*this, m_event);

            // ..and notify it.
            op->setReceived(match);
            op->getNotifier().notify(*op);
        } else {
            // Skip it
            ++it;
        }
    }

    // Claim we did NOT handle it.
    // This means caller will not do cleanup, which we already did.
    return false;
}

void
arch::win32::Win32Interrupt::setupOnce(InterruptOperation_t& op, InterruptOperation_t::Kind k)
{
    if (op.getRequested().contains(k)) {
        if (!m_attached[k]) {
            InterlockedExchangeAdd(&m_attached[k], 1);
        }
    }
}
#else
int g_variableToMakeWin32InterruptObjectFileNotEmpty;
#endif
