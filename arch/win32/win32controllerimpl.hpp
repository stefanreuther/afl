/**
  *  \file arch/win32/win32controllerimpl.hpp
  *  \brief Class arch::win32::Win32ControllerImpl
  */
#ifndef AFL_ARCH_WIN32_WIN32CONTROLLERIMPL_HPP
#define AFL_ARCH_WIN32_WIN32CONTROLLERIMPL_HPP

#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <list>
#include <windows.h>
#include "afl/sys/types.hpp"

namespace arch { namespace win32 {

    class WaitRequest;

    /** Implementation of Controller::Impl for Win32.
        Implements synchronisation using a set of HANDLE's and WaitForMultipleObjects. */
    class Win32ControllerImpl {
     public:
        Win32ControllerImpl();
        ~Win32ControllerImpl();

        void wait(afl::sys::Timeout_t timeout);
        void wake();
        void prepare();
        void finish();

        void addRequest(WaitRequest& req, HANDLE h);
        void removeRequest(WaitRequest& req, HANDLE h);

     private:
        HANDLE m_event;

        struct Request {
            WaitRequest* m_pWaitRequest;
            HANDLE m_handle;

            Request(WaitRequest& waitRequest, HANDLE handle)
                : m_pWaitRequest(&waitRequest),
                  m_handle(handle)
                { }
        };

        std::list<Request> m_requests;
    };

} }

#endif
