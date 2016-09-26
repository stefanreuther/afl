/**
  *  \file afl/net/headerconsumer.hpp
  *  \brief Interface afl::net::HeaderConsumer
  */
#ifndef AFL_AFL_NET_HEADERCONSUMER_HPP
#define AFL_AFL_NET_HEADERCONSUMER_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net {

    /** Interface for consuming header information.
        This is used by HeaderParser to report parsed headers. */
    class HeaderConsumer : public afl::base::Deletable {
     public:
        virtual void handleHeader(String_t key, String_t value) = 0;
    };

} }

#endif
