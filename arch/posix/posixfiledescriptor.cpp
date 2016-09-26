/**
  *  \file arch/posix/posixfiledescriptor.cpp
  *  \brief Class arch::posix::PosixFileDescriptor
  */

#if TARGET_OS_POSIX
#include "arch/posix/posixfiledescriptor.hpp"

#include <errno.h>
#include <unistd.h>
#include <sys/select.h>         // select
#include <sys/time.h>           // struct timeval
#include "afl/except/systemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/error.hpp"
#include "afl/sys/mutexguard.hpp"
#include "arch/controller.hpp"
#include "arch/posix/posixcontrollerimpl.hpp"
#include "afl/async/notifier.hpp"

bool
arch::posix::PosixFileDescriptor::send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& op, afl::sys::Timeout_t timeout)
{
    afl::base::ConstBytes_t m(op.getUnsentBytes());
    if (m.size() == 0) {
        return true;
    } else if (waitReady(m_fd, timeout, WaitForWrite)) {
        ssize_t n = ::write(m_fd, m.unsafeData(), m.size());
        if (n < 0) {
            // FIXME: can we throw here?
            throw afl::except::SystemException(afl::sys::Error::current(), "send");
        }
        op.addSentBytes(n);
        return true;
    } else {
        return false;
    }
}

void
arch::posix::PosixFileDescriptor::sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    afl::async::Controller::Impl& pi = ctl.getImplementation();
    if (m_pendingSends.empty()) {
        // FIXME: this is wrong! must always add to controller!
        pi.addRequest(*this, m_fd, false);
    }
    op.setController(&ctl);
    m_pendingSends.pushBack(&op);
}

bool
arch::posix::PosixFileDescriptor::receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout)
{
    afl::base::Bytes_t m(op.getUnreceivedBytes());
    if (m.size() == 0) {
        return true;
    } else if (waitReady(m_fd, timeout, WaitForRead)) {
        ssize_t n = ::read(m_fd, m.unsafeData(), m.size());
        if (n < 0) {
            // FIXME: can we throw here?
            throw afl::except::SystemException(afl::sys::Error::current(), "receive");
        }
        op.addReceivedBytes(n);
        return true;
    } else {
        return false;
    }
}

void
arch::posix::PosixFileDescriptor::receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    arch::posix::PosixControllerImpl& pi = ctl.getImplementation();
    if (m_pendingReceives.empty()) {
        // FIXME: this is wrong! must always add to controller!
        pi.addRequest(*this, m_fd, true);
    }
    op.setController(&ctl);
    m_pendingReceives.pushBack(&op);
}

void
arch::posix::PosixFileDescriptor::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_pendingSends.remove(&op);
    m_pendingReceives.remove(&op);
    ctl.revertPost(op);

    // FIXME: what to do with the SelectRequest?
}

bool
arch::posix::PosixFileDescriptor::waitReady(int fd, afl::sys::Timeout_t timeout, WaitMode mode)
{
    // FIXME: handle FD_SETSIZE
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);

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

    // System call
    errno = 0;
    int result = mode == WaitForRead
        ? ::select(fd + 1, &set, 0, 0, ptv)
        : ::select(fd + 1, 0, &set, 0, ptv);

    // Return
    if (result > 0 && FD_ISSET(fd, &set)) {
        return true;
    } else {
        return false;
    }
}

bool
arch::posix::PosixFileDescriptor::handleReadReady()
{
    afl::sys::MutexGuard g(m_mutex);
    while (afl::async::ReceiveOperation* op = m_pendingReceives.front()) {
        // How much remains?
        afl::base::Bytes_t buf = op->getUnreceivedBytes();

        // Call read()
        ssize_t n = buf.empty() ? 0 : ::read(m_fd, buf.unsafeData(), buf.size());
        if (n > 0) {
            op->addReceivedBytes(n);
        }

        // Exit when getting EAGAIN, this means data is exhausted
        if (n < 0 && errno == EAGAIN) {
            break;
        }

        // For now, return immediately, even partial reads.
        // This would check for completed only (or EOF): "if (op->isCompleted() || n <= 0)"
        op->getNotifier().notify(*op);
        m_pendingReceives.extractFront();
    }

    return m_pendingReceives.empty();
}

bool
arch::posix::PosixFileDescriptor::handleWriteReady()
{
    afl::sys::MutexGuard g(m_mutex);
    while (afl::async::SendOperation* op = m_pendingSends.front()) {
        // How much remains?
        afl::base::ConstBytes_t buf = op->getUnsentBytes();

        // Call read()
        ssize_t n = buf.empty() ? 0 : ::write(m_fd, buf.unsafeData(), buf.size());
        if (n > 0) {
            op->addSentBytes(n);
        }

        // Exit when getting EAGAIN, this means data is exhausted
        if (n < 0 && errno == EAGAIN) {
            break;
        }

        // For now, return immediately, even partial writes.
        // This would check for completed only: "if (op->isCompleted() || n <= 0)"
        op->getNotifier().notify(*op);
        m_pendingSends.extractFront();
    }

    return m_pendingSends.empty();
}
#else
int g_variableToMakePosixFileDescriptorObjectFileNotEmpty;
#endif
