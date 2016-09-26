/**
  *  \file afl/except/unsupportedexception.hpp
  *  \brief Class afl::except::UnsupportedException
  */
#ifndef AFL_AFL_EXCEPT_UNSUPPORTEDEXCEPTION_HPP
#define AFL_AFL_EXCEPT_UNSUPPORTEDEXCEPTION_HPP

#include <stdexcept>
#include "afl/string/string.hpp"

namespace afl { namespace except {

    /** Unsupported feature exception.
        Reports that an unsupported feature was attempted.
        This could be a feature not configured in, or not implemented. */
    class UnsupportedException : public std::runtime_error {
     public:
        /** Constructor.
            \param feature Feature name */
        UnsupportedException(const String_t& feature);

        /** Destructor. */
        ~UnsupportedException() throw();
    };

} }

#endif
