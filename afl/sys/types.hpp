/**
  *  \file afl/sys/types.hpp
  *  \brief Fundamental Types
  */
#ifndef AFL_AFL_SYS_TYPES_HPP
#define AFL_AFL_SYS_TYPES_HPP

#include "afl/base/types.hpp"

namespace afl { namespace sys {

    /** Timeout. Specified in milliseconds. */
    typedef uint32_t Timeout_t;

    /** Timeout signifying "infinity". */
    const Timeout_t INFINITE_TIMEOUT = static_cast<Timeout_t>(-1);

} }

#endif
