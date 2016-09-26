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
        // Look up element
        std::list<Request>::iterator it = m_requests.begin();
        for (DWORD i = 0; i < result - WAIT_OBJECT_0 - 1 && it != m_requests.end(); ++i) {
            ++it;
        }

        // Handle it
        if (it != m_requests.end() && it->m_pWaitRequest->handleWaitReady()) {
            m_requests.erase(it);
        }
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
#else
int g_variableToMakeWin32ControllerImplObjectFileNotEmpty;
#endif
