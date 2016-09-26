/**
  *  \file arch/posix/posixcontrollerimpl.cpp
  *  \brief Class arch::posix::PosixControllerImpl
  */

#if TARGET_OS_POSIX
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>         // select
#include <errno.h>
#include <signal.h>
#include "arch/posix/posixcontrollerimpl.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/sys/error.hpp"
#include "arch/posix/selectrequest.hpp"

namespace {
    bool g_beenHere;
}

arch::posix::PosixControllerImpl::PosixControllerImpl()
    : m_mode(NotWaiting),
      m_sem(0),
      m_requests()
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
}

arch::posix::PosixControllerImpl::~PosixControllerImpl()
{
    ::close(m_fds[0]);
    ::close(m_fds[1]);
}

void
arch::posix::PosixControllerImpl::wait(afl::sys::Timeout_t timeout)
{
    if (m_mode == WaitingForFileDescriptor) {
        // Expensive wait using file descriptors
        fd_set readSet;
        fd_set writeSet;
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(m_fds[0], &readSet);
        int max = m_fds[0];

        // Add all requests
        for (std::list<Request>::iterator it = m_requests.begin(); it != m_requests.end(); ++it) {
            if (it->m_read) {
                FD_SET(it->m_fd, &readSet);
            } else {
                FD_SET(it->m_fd, &writeSet);
            }
            if (it->m_fd > max) {
                max = it->m_fd;
            }
        }

        // Prepare timeout
        struct timeval tv;
        struct timeval* ptv;
        if (timeout == afl::sys::INFINITE_TIMEOUT) {
            ptv = 0;
        } else {
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;
            ptv = &tv;
        }

        // Wait
        int n = ::select(max+1, &readSet, &writeSet, 0, ptv);
        if (n > 0) {
            // Drain "wake me" pipe
            if (FD_ISSET(m_fds[0], &readSet)) {
                char tmp[32];
                while (::read(m_fds[0], tmp, sizeof(tmp)) > 0) {
                    /* nix */
                }
            }

            // Since I'm out of the select now, there's no need for externals to poke me.
            m_mode = NotWaiting;

            // Check waiters
            std::list<Request>::iterator it = m_requests.begin();
            while (it != m_requests.end()) {
                bool done = (it->m_read
                             ? FD_ISSET(it->m_fd, &readSet)  && it->m_pSelectRequest->handleReadReady()
                             : FD_ISSET(it->m_fd, &writeSet) && it->m_pSelectRequest->handleWriteReady());
                if (done) {
                    it = m_requests.erase(it);
                } else {
                    ++it;
                }
            }

            // I might enter the select again, tell others that I might need a kick.
            m_mode = WaitingForFileDescriptor;
        }
    } else {
        // Cheap wait using semaphores
        m_sem.wait(timeout);
    }
}

void
arch::posix::PosixControllerImpl::wake()
{
    switch (m_mode) {
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
        m_mode = WaitingForSemaphore;
    } else {
        m_mode = WaitingForFileDescriptor;
    }
}

void
arch::posix::PosixControllerImpl::finish()
{
    m_mode = NotWaiting;
}

void
arch::posix::PosixControllerImpl::addRequest(SelectRequest& req, int fd, bool read)
{
    m_requests.push_back(Request(req, fd, read));
}
#else
int g_variableToMakePosixControllerImplObjectFileNotEmpty;
#endif
