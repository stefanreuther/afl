/**
  *  \file arch/win32/win32controllerimpl.cpp
  *  \brief Class arch::win32::Win32ControllerImpl
  */

#if TARGET_OS_WIN32
#include "arch/win32/win32controllerimpl.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/sys/error.hpp"
#include "arch/win32/waitrequest.hpp"

arch::win32::Win32ControllerImpl::Win32ControllerImpl()
    : m_event(CreateEvent(0, FALSE, FALSE, 0)),
      m_cancelState(Idle),
      m_requests()
{
    if (m_event == NULL) {
        throw afl::except::SystemException(afl::sys::Error::current(), "<CreateEvent>");
    }
}

arch::win32::Win32ControllerImpl::~Win32ControllerImpl()
{
    CloseHandle(m_event);
}

void
arch::win32::Win32ControllerImpl::wait(afl::sys::Timeout_t timeout)
{
    // Build handle list.
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    DWORD n = 0;

    // First our own handle
    handles[n++] = m_event;

    // Now our children
    for (std::list<Request>::iterator it = m_requests.begin(); it != m_requests.end(); ++it) {
        handles[n++] = it->m_handle;
        if (n >= MAXIMUM_WAIT_OBJECTS) {
            // FIXME: handle overflow!
            break;
        }
    }

    // Wait
    DWORD result = WaitForMultipleObjects(n, handles, FALSE, timeout == afl::sys::INFINITE_TIMEOUT ? INFINITE : timeout);

    // Evaluate result
    // We do not care about WAIT_OBJECT_0, because that is our own handle.
    if (result >= WAIT_OBJECT_0 + 1 && result < WAIT_OBJECT_0 + n) {
        // Block cancellations in case handleWaitReady calls removeRequest.
        if (m_cancelState == Idle) {
            m_cancelState = Blocked;
        }

        // Look up element
        const HANDLE foundHandle = handles[result - WAIT_OBJECT_0];
        std::list<Request>::iterator it = m_requests.begin();
        while (it != m_requests.end()) {
            if (it->m_handle == foundHandle && !it->m_cancelled) {
                if (it->m_pWaitRequest->handleWaitReady()) {
                    m_requests.erase(it);
                }
                break;
            } else {
                ++it;
            }
        }

        // Perform deferred cancellations
        if (m_cancelState == Dirty) {
            std::list<Request>::iterator it = m_requests.begin();
            while (it != m_requests.end()) {
                if (it->m_cancelled) {
                    it = m_requests.erase(it);
                } else {
                    ++it;
                }
            }
        }
        m_cancelState = Idle;
    }
}

void
arch::win32::Win32ControllerImpl::wake()
{
    SetEvent(m_event);
}

void
arch::win32::Win32ControllerImpl::prepare()
{ }

void
arch::win32::Win32ControllerImpl::finish()
{ }

void
arch::win32::Win32ControllerImpl::addRequest(WaitRequest& req, HANDLE h)
{
    m_requests.push_back(Request(req, h));
}

void
arch::win32::Win32ControllerImpl::removeRequest(WaitRequest& req, HANDLE h)
{
    for (std::list<Request>::iterator it = m_requests.begin(); it != m_requests.end(); ++it) {
        if (it->m_pWaitRequest == &req && it->m_handle == h && !it->m_cancelled) {
            if (m_cancelState == Idle) {
                m_requests.erase(it);
            } else {
                it->m_cancelled = true;
                m_cancelState = Dirty;
            }
            break;
        }
    }
}

#else
int g_variableToMakeWin32ControllerImplObjectFileNotEmpty;
#endif
