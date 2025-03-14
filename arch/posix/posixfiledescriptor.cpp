/**
  *  \file arch/posix/posixfiledescriptor.cpp
  *  \brief Class arch::posix::PosixFileDescriptor
  */

#if TARGET_OS_POSIX
#include "arch/posix/posixfiledescriptor.hpp"

#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>
#include "afl/async/notifier.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/error.hpp"
#include "afl/sys/mutexguard.hpp"
#include "arch/controller.hpp"
#include "arch/posix/posixcontrollerimpl.hpp"

bool
arch::posix::PosixFileDescriptor::send(afl::async::Controller& /*ctl*/, afl::async::SendOperation& op, afl::sys::Timeout_t timeout)
{
    afl::base::ConstBytes_t m(op.getUnsentBytes());
    if (m.size() == 0) {
        return true;
    } else if (waitReady(m_pImpl->fd, timeout, WaitForWrite)) {
        ssize_t n = ::write(m_pImpl->fd, m.unsafeData(), m.size());
        if (n < 0) {
            // FIXME: can we throw here?
            throw afl::except::SystemException(afl::sys::Error::current(), "send");
        }
        op.addSentBytes(size_t(n));
        return true;
    } else {
        return false;
    }
}

void
arch::posix::PosixFileDescriptor::sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op)
{
    afl::base::Ref<Content> self(m_pImpl);
    afl::sys::MutexGuard g(self->mutex);
    afl::async::Controller::Impl& pi = ctl.getImplementation();
    op.setController(&ctl);
    pi.addRequest(*this, op, self->fd, false);
    self->pendingSends.pushBack(&op);
}

bool
arch::posix::PosixFileDescriptor::receive(afl::async::Controller& /*ctl*/, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout)
{
    afl::base::Bytes_t m(op.getUnreceivedBytes());
    if (m.size() == 0) {
        return true;
    } else if (waitReady(m_pImpl->fd, timeout, WaitForRead)) {
        ssize_t n = ::read(m_pImpl->fd, m.unsafeData(), m.size());
        if (n < 0) {
            // FIXME: can we throw here?
            throw afl::except::SystemException(afl::sys::Error::current(), "receive");
        }
        op.addReceivedBytes(size_t(n));
        return true;
    } else {
        return false;
    }
}

void
arch::posix::PosixFileDescriptor::receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op)
{
    afl::base::Ref<Content> self(m_pImpl);
    afl::sys::MutexGuard g(self->mutex);
    arch::posix::PosixControllerImpl& pi = ctl.getImplementation();
    op.setController(&ctl);
    pi.addRequest(*this, op, self->fd, true);
    self->pendingReceives.pushBack(&op);
}

void
arch::posix::PosixFileDescriptor::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    afl::base::Ref<Content> self(m_pImpl);
    afl::sys::MutexGuard g(self->mutex);
    self->pendingSends.remove(&op);
    self->pendingReceives.remove(&op);
    ctl.revertPost(op);
    ctl.getImplementation().removeRequest(op);
}

bool
arch::posix::PosixFileDescriptor::waitReady(int fd, afl::sys::Timeout_t timeout, WaitMode mode)
{
    // Prepare struct pollfd
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = (mode == WaitForRead ? POLLIN : POLLOUT);
    pfd.revents = 0;

    // Prepare timeout
    int convertedTimeout = convertTimeout(timeout);

    // System call
    errno = 0;
    int result = ::poll(&pfd, 1, convertedTimeout);

    // Return
    if (result > 0 && pfd.revents != 0) {
        return true;
    } else {
        return false;
    }
}

int
arch::posix::PosixFileDescriptor::convertTimeout(afl::sys::Timeout_t timeout)
{
    if (timeout == afl::sys::INFINITE_TIMEOUT) {
        // Negative means infinite
        return -1;
    } else if (timeout > INT_MAX) {
        // Overflows
        return INT_MAX;
    } else {
        // Fits
        return static_cast<int>(timeout);
    }
}

bool
arch::posix::PosixFileDescriptor::handleReadReady()
{
    /*
     *  This is pretty delicate because it must deal with a callback adding/removing more instances of the same object.
     *  The system test case for exercising this is the SOCKS "accept" operation:
     *  if the remote end (SOCKS server) dies, this will cause a socket to be closed,
     *  which originally caused a deleted object access (Valgrind).
     *
     *  - Use Ref<Content> to secure access to the data in case the callback deletes this object.
     *    (This could possibly be removed by switching to fine-granular locking and calling the notifier outside the lock.)
     *  - Process only one callback. This used to optimize and process multiple callbacks.
     *    If a callback adds a new request, that one could be processed in the same run through this loop
     *    as the original callback, leading to confusion about when to add/remove the SelectRequest to the PosixControllerImpl.
     *  - Remove callback from queue before invoking it.
     */
    afl::base::Ref<Content> self(m_pImpl);
    afl::sys::MutexGuard g(self->mutex);
    bool result = true;
    if (afl::async::ReceiveOperation* op = self->pendingReceives.front()) {
        // How much remains?
        afl::base::Bytes_t buf = op->getUnreceivedBytes();

        // Call read()
        ssize_t n = buf.empty() ? 0 : ::read(self->fd, buf.unsafeData(), buf.size());
        if (n > 0) {
            op->addReceivedBytes(size_t(n));
        }

        // Exit when getting EAGAIN, this means data is exhausted
        if (n < 0 && errno == EAGAIN) {
            return false;
        }

        // For now, return immediately, even partial reads.
        // This would check for completed only (or EOF): "if (op->isCompleted() || n <= 0)"
        // Remove from queue first, in case notify() modifies the queue.
        self->pendingReceives.extractFront();
        op->getNotifier().notify(*op);
    }

    return result;
}

bool
arch::posix::PosixFileDescriptor::handleWriteReady()
{
    // See handleReadReady().
    afl::base::Ref<Content> self(m_pImpl);
    afl::sys::MutexGuard g(self->mutex);
    bool result = true;
    if (afl::async::SendOperation* op = self->pendingSends.front()) {
        // How much remains?
        afl::base::ConstBytes_t buf = op->getUnsentBytes();

        // Call write()
        ssize_t n = buf.empty() ? 0 : ::write(self->fd, buf.unsafeData(), buf.size());
        if (n > 0) {
            op->addSentBytes(size_t(n));
        }

        // Exit when getting EAGAIN, this means data is exhausted
        if (n < 0 && errno == EAGAIN) {
            return false;
        }

        // For now, return immediately, even partial writes.
        // This would check for completed only: "if (op->isCompleted() || n <= 0)"
        self->pendingSends.extractFront();
        op->getNotifier().notify(*op);
    }

    return result;
}
#else
int g_variableToMakePosixFileDescriptorObjectFileNotEmpty;
#endif
