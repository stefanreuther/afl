/**
  *  \file arch/posix/posixcontrollerimpl.hpp
  *  \brief Class arch::posix::PosixControllerImpl
  */
#ifndef AFL_ARCH_POSIX_POSIXCONTROLLERIMPL_HPP
#define AFL_ARCH_POSIX_POSIXCONTROLLERIMPL_HPP

#include <list>
#include "afl/sys/semaphore.hpp"

namespace arch { namespace posix {

    class SelectRequest;

    /** Implementation of Controller::Impl for POSIX. */
    class PosixControllerImpl {
     public:
        PosixControllerImpl();
        ~PosixControllerImpl();

        void wait(afl::sys::Timeout_t timeout);
        void wake();
        void prepare();
        void finish();

        void addRequest(SelectRequest& req, int fd, bool read);

     private:
        enum Mode {
            NotWaiting,
            WaitingForSemaphore,
            WaitingForFileDescriptor
        };
        Mode m_mode;

        int m_fds[2];
        afl::sys::Semaphore m_sem;

        struct Request {
            SelectRequest* m_pSelectRequest;
            int m_fd;
            bool m_read;

            Request(SelectRequest& selectRequest, int fd, bool read)
                : m_pSelectRequest(&selectRequest),
                  m_fd(fd),
                  m_read(read)
                { }
        };

        std::list<Request> m_requests;
    };

} }

#endif
