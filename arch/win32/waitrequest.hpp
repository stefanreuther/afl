/**
  *  \file arch/win32/waitrequest.hpp
  *  \brief Interface arch::win32::WaitRequest
  */
#ifndef AFL_ARCH_WIN32_WAITREQUEST_HPP
#define AFL_ARCH_WIN32_WAITREQUEST_HPP

namespace arch { namespace win32 {

    class WaitRequest {
     public:
        virtual bool handleWaitReady() = 0;
    };

} }

#endif
