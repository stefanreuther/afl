/**
  *  \file arch/posix/posixfiledescriptor.hpp
  *  \brief Class arch::posix::PosixFileDescriptor
  */
#ifndef AFL_ARCH_POSIX_POSIXFILEDESCRIPTOR_HPP
#define AFL_ARCH_POSIX_POSIXFILEDESCRIPTOR_HPP

#include <unistd.h>
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/sys/mutex.hpp"
#include "arch/posix/selectrequest.hpp"
#include "afl/sys/types.hpp"

#include "afl/base/ref.hpp"
#include "afl/base/refcounted.hpp"

namespace arch { namespace posix {

    /** Implementation of a file descriptor for POSIX.
        This is the underlying base class for asynchronous I/O using file descriptors. */
    class PosixFileDescriptor : private arch::posix::SelectRequest {
     public:
        PosixFileDescriptor(int fd)
            : m_pImpl(*new Content(fd))
            { }

        virtual ~PosixFileDescriptor()
            { }

        int getFileDescriptor() const
            { return m_pImpl->fd; }

        bool send(afl::async::Controller& ctl, afl::async::SendOperation& op, afl::sys::Timeout_t timeout);
        void sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op);
        bool receive(afl::async::Controller& ctl, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout);
        void receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op);
        void cancel(afl::async::Controller& ctl, afl::async::Operation& op);

        // Utility: Helper for poll() syscall
        enum WaitMode {
            WaitForRead,
            WaitForWrite
        };
        static bool waitReady(int fd, afl::sys::Timeout_t timeout, WaitMode mode);
        static int convertTimeout(afl::sys::Timeout_t timeout);

     private:
        bool handleReadReady();
        bool handleWriteReady();

        struct Content : public afl::base::RefCounted {
            int fd;
            afl::sys::Mutex mutex;
            afl::async::OperationList<afl::async::SendOperation> pendingSends;
            afl::async::OperationList<afl::async::ReceiveOperation> pendingReceives;
            Content(int fd)
                : fd(fd), mutex(), pendingSends(), pendingReceives()
                { }
            ~Content()
                { ::close(fd); }
        };
        afl::base::Ref<Content> m_pImpl;
    };

} }

#endif
