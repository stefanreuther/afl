/**
  *  \file arch/posix/posixcontrollerimpl.hpp
  *  \brief Class arch::posix::PosixControllerImpl
  */
#ifndef AFL_ARCH_POSIX_POSIXCONTROLLERIMPL_HPP
#define AFL_ARCH_POSIX_POSIXCONTROLLERIMPL_HPP

#include <list>
#include <vector>
#include <poll.h>
#include "afl/sys/semaphore.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/async/operation.hpp"

namespace arch { namespace posix {

    class SelectRequest;

    /** Implementation of Controller::Impl for POSIX.
        It can wait for file descriptors, and be woken up from external events.
        It has three modes of operation:
        - NotWaiting: the thread this Controller belongs to does not currently wait for anything.
          Nothing needs to be done to wake it because it is not sleeping.
        - WaitingForSemaphore: sleeping using a semaphore because no file descriptors are being waited on.
          To wake, post the semaphore.
        - WaitingForFileDescriptor: sleeping using poll() on a bunch of file descriptors.
          A pipe has been created in addition; write a byte to wake it. */
    class PosixControllerImpl {
     public:
        PosixControllerImpl();
        ~PosixControllerImpl();

        void wait(afl::sys::Timeout_t timeout);
        void wake();
        void prepare();
        void finish();

        void addRequest(SelectRequest& req, afl::async::Operation& op, int fd, bool read);
        void removeRequest(afl::async::Operation& op);

     private:
        /*
         *  Mode
         *
         *  The mode is protected by a mutex.
         *  We don't actually need the "mutual exclusion" functionality, but we need a memory barrier.
         *  On a single-processor system or one with strict memory ordering, the mutex is not needed.
         */
        enum Mode {
            NotWaiting,
            WaitingForSemaphore,
            WaitingForFileDescriptor
        };
        Mode m_mode;
        afl::sys::Mutex m_modeMutex;

        void setMode(Mode m);
        Mode getMode();

        /*
         *  Waking me up
         */

        int m_fds[2];
        afl::sys::Semaphore m_sem;


        /*
         *  User Requests
         *
         *  Each request adds an element to the request list.
         *  If the request is cancelled by the user, it will be removed from the request list.
         *  (a) to identify it, we use the user's Operation object which identifies this request uniquely.
         *      The other parameters may not be unique, for example, multiple requests on one fd.
         *  (b) because an object's cancel() is allowed to be called anytime, including from a completion callback,
         *      we must deal with removeRequest() being called while we are iterating the request list.
         *      Use CancelState to defer actual removal in this case.
         */

        enum CancelState {
            Idle,               // Normal state
            Blocked,            // Do not remove items from m_requests; only flag them for deletion
            Dirty               // Items were flagged for deletion; postprocessing of the list required
        };
        CancelState m_cancelState;

        struct Request {
            SelectRequest* m_pSelectRequest;
            afl::async::Operation* m_pOperation;
            int m_fd;
            bool m_read;
            bool m_cancelled;   // Treat item as deleted

            Request(SelectRequest& selectRequest, afl::async::Operation& op, int fd, bool read)
                : m_pSelectRequest(&selectRequest),
                  m_pOperation(&op),
                  m_fd(fd),
                  m_read(read),
                  m_cancelled(false)
                { }
        };

        /** List of pending requests. */
        std::list<Request> m_requests;

        /** Storage for struct pollfd.
            Storing this as an instance variable avoids having to allocate memory in each wait();
            it will only allocate to enlarge and then re-use the storage for future calls. */
        std::vector<struct pollfd> m_pollfds;
    };

} }

#endif
