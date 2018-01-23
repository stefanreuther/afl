/**
  *  \file arch/posix/posixcontrollerimpl.cpp
  *  \brief Class arch::posix::PosixControllerImpl
  */

#if TARGET_OS_POSIX
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include "arch/posix/posixcontrollerimpl.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/sys/error.hpp"
#include "afl/sys/mutexguard.hpp"
#include "arch/posix/posixfiledescriptor.hpp"
#include "arch/posix/selectrequest.hpp"

namespace {
    bool g_beenHere;
}

arch::posix::PosixControllerImpl::PosixControllerImpl()
    : m_mode(NotWaiting),
      m_modeMutex(),
      m_sem(0),
      m_cancelState(Idle),
      m_requests(),
      m_pollfds()
{
    // SIGPIPE will kill us so we must disable it.
    if (!g_beenHere) {
        g_beenHere = true;
        signal(SIGPIPE, SIG_IGN);
    }
    if (::pipe(m_fds) != 0) {
        throw afl::except::SystemException(afl::sys::Error::current(), "<pipe>");
    }
    ::fcntl(m_fds[0], F_SETFL, O_NONBLOCK);
    ::fcntl(m_fds[1], F_SETFL, O_NONBLOCK);
    ::fcntl(m_fds[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(m_fds[1], F_SETFD, FD_CLOEXEC);
}

arch::posix::PosixControllerImpl::~PosixControllerImpl()
{
    ::close(m_fds[0]);
    ::close(m_fds[1]);
}

void
arch::posix::PosixControllerImpl::wait(afl::sys::Timeout_t timeout)
{
    if (getMode() == WaitingForFileDescriptor) {
        // Expensive wait using file descriptors
        m_pollfds.resize(1 + m_requests.size());
        m_pollfds[0].fd = m_fds[0];
        m_pollfds[0].events = POLLIN;
        m_pollfds[0].revents = 0;

        // Add all requests
        size_t i = 1;
        for (std::list<Request>::iterator it = m_requests.begin(); it != m_requests.end(); ++it) {
            m_pollfds[i].fd = it->m_fd;
            m_pollfds[i].events = it->m_read ? POLLIN : POLLOUT;
            m_pollfds[i].revents = 0;
            ++i;
        }

        // Prepare timeout
        int convertedTimeout = PosixFileDescriptor::convertTimeout(timeout);

        // Wait
        int n = ::poll(&m_pollfds[0], m_pollfds.size(), convertedTimeout);
        if (n > 0) {
            // Drain "wake me" pipe
            if (m_pollfds[0].revents != 0) {
                char tmp[32];
                while (::read(m_fds[0], tmp, sizeof(tmp)) > 0) {
                    /* nix */
                }
            }

            // Since I'm out of the poll() now, there's no need for externals to poke me.
            setMode(NotWaiting);

            // Block cancellations in case handleReadReady/handleWriteReady calls removeRequest.
            if (m_cancelState == Idle) {
                m_cancelState = Blocked;
            }

            // Check waiters
            std::list<Request>::iterator it = m_requests.begin();
            i = 1;
            while (it != m_requests.end()) {
                bool done = (m_pollfds[i].revents != 0
                             && (it->m_read
                                 ? it->m_pSelectRequest->handleReadReady()
                                 : it->m_pSelectRequest->handleWriteReady()));
                if (done) {
                    it = m_requests.erase(it);
                } else {
                    ++it;
                }
                ++i;
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

            // I might enter the poll() again, tell others that I might need a kick.
            // 20171124: I don't think that is needed (anymore?), it is set in prepare().
            // m_mode = WaitingForFileDescriptor;
        } else {
            setMode(NotWaiting);
        }
    } else {
        // Cheap wait using semaphores
        m_sem.wait(timeout);
    }
}

void
arch::posix::PosixControllerImpl::wake()
{
    switch (getMode()) {
     case NotWaiting:
        // Nothing to do, other side does not wait
        break;

     case WaitingForSemaphore:
        // Simple
        m_sem.post();
        break;

     case WaitingForFileDescriptor:
        // Wake other side by writing a byte.
        // We don't care whether the write succeeds;
        // if it doesn't, this means the pipe is full and the other side will wake up soon.
        {
            ssize_t n = ::write(m_fds[1], "", 1);
            (void) n;
        }
        break;
    }
}

void
arch::posix::PosixControllerImpl::prepare()
{
    if (m_requests.empty()) {
        setMode(WaitingForSemaphore);
    } else {
        setMode(WaitingForFileDescriptor);
    }
}

void
arch::posix::PosixControllerImpl::finish()
{
    setMode(NotWaiting);
}

void
arch::posix::PosixControllerImpl::addRequest(SelectRequest& req, afl::async::Operation& op, int fd, bool read)
{
    m_requests.push_back(Request(req, op, fd, read));
}

void
arch::posix::PosixControllerImpl::removeRequest(afl::async::Operation& op)
{
    for (std::list<Request>::iterator it = m_requests.begin(); it != m_requests.end(); ++it) {
        if (it->m_pOperation == &op && !it->m_cancelled) {
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

inline void
arch::posix::PosixControllerImpl::setMode(Mode m)
{
    afl::sys::MutexGuard g(m_modeMutex);
    m_mode = m;
}

inline arch::posix::PosixControllerImpl::Mode
arch::posix::PosixControllerImpl::getMode()
{
    afl::sys::MutexGuard g(m_modeMutex);
    return m_mode;
}
#else
int g_variableToMakePosixControllerImplObjectFileNotEmpty;
#endif
