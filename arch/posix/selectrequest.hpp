/**
  *  \file arch/posix/selectrequest.hpp
  *  \brief Interface arch::posix::SelectRequest
  */
#ifndef AFL_ARCH_POSIX_SELECTREQUEST_HPP
#define AFL_ARCH_POSIX_SELECTREQUEST_HPP

namespace arch { namespace posix {

    /** A request to select().
        This is the callback. */
    class SelectRequest {
     public:
        /** Called when reading is possible.
            \retval true remove the request
            \retval false keep the request, keep trying to read */
        virtual bool handleReadReady() = 0;

        /** Called when reading is possible.
            \retval true remove the request
            \retval false keep the request, keep trying to write */
        virtual bool handleWriteReady() = 0;
    };

} }

#endif
