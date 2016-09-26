/**
  *  \file afl/except/systemexception.hpp
  *  \brief Class afl::except::SystemException
  */
#ifndef AFL_AFL_EXCEPT_SYSTEMEXCEPTION_HPP
#define AFL_AFL_EXCEPT_SYSTEMEXCEPTION_HPP

#include <stdexcept>
#include "afl/sys/error.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace except {

    /** System exception.
        Reports a failing syscall.
        Contains an error code (afl::sys::Error), and an optional hint.
        The hint usually is a syscall name giving a hint about the location of the problem;
        it is not intended to be end-user-friendly. */
    class SystemException : public std::runtime_error {
     public:
        /** Construct from error.
            The exception will not have a hint.
            \param error The error */
        SystemException(afl::sys::Error error);

        /** Construct from error and hint.
            \param error The error
            \param hint Hint. Usually, a syscall name. */
        SystemException(afl::sys::Error error, const String_t& hint);

        /** Destructor. */
        ~SystemException() throw();

        /** Get hint.
            \return The hint this was instantiated with. */
        const String_t& getHint() const;

        /** Get error code.
            \return The error code this was instantiated with. */
        afl::sys::Error getError() const;

     private:
        String_t m_hint;
        afl::sys::Error m_error;
    };

} }

inline const String_t&
afl::except::SystemException::getHint() const
{
    return m_hint;
}

inline afl::sys::Error
afl::except::SystemException::getError() const
{
    return m_error;
}

#endif
