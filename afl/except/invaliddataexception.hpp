/**
  *  \file afl/except/invaliddataexception.hpp
  *  \brief Class afl::except::InvalidDataException
  */
#ifndef AFL_AFL_EXCEPT_INVALIDDATAEXCEPTION_HPP
#define AFL_AFL_EXCEPT_INVALIDDATAEXCEPTION_HPP

#include <stdexcept>
#include "afl/string/string.hpp"

namespace afl { namespace except {

    /** Invalid data exception.
        Reports invalid data provided somewhere.

        For invalid data from a stream, FileFormatException could be used. */
    class InvalidDataException : public std::runtime_error {
     public:
        /** Constructor.
            \param msg Message */
        InvalidDataException(const String_t& msg);

        /** Destructor. */
        ~InvalidDataException() throw();
    };

} }

#endif
