/**
  *  \file afl/sys/error.hpp
  *  \brief Class afl::sys::Error
  */
#ifndef AFL_AFL_SYS_ERROR_HPP
#define AFL_AFL_SYS_ERROR_HPP

#include "afl/base/types.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace sys {

    /** Representation of a System Error.
        Systems report errors in a variety of ways.
        We assume they do so using error codes that fit in 32 bits.
        This applies to POSIX (errno), Win32 (GetLastError()), and a most others in some way. */
    class Error {
     public:
        /** Error number.
            0 means no error, otherwise it's an error code. */
        typedef int32_t ErrorNumber_t;

        /** Construct "no error" value. */
        Error();

        /** Construct value from error code supplied by operating system.
            \param code Error code */
        explicit Error(ErrorNumber_t code);

        /** Create Error from current error code.
            If a syscall reports an error,
            but doesn't immediately provide the value to pass to the constructor,
            this function will create the Error object corresponding to the syscall error. */
        static Error current();

        /** Get error number.
            For informative purposes (because you cannot portably know the error numbers. */
        ErrorNumber_t getNumber() const;

        /** Get error text.
            \return error text, in UTF-8 encoding. */
        String_t getText();

        /** Validity check.
            For use in code like 'if (error)'.
            \retval true This is an error
            \retval false This is not an error */
        operator bool() const;

        /** Compare for equality.
            \param other Other error
            \return true if both errors have the same number. */
        bool operator==(const Error other) const;

        /** Compare for inequality.
            \param other Other error
            \return true if both errors have different numbers. */
        bool operator!=(const Error other) const;

        /** Compare for less-than.
            \param other Other error
            \return true if this error has a lower number than the other. */
        bool operator<(const Error other) const;

        /** Compare for greater-than.
            \param other Other error
            \return true if this error has a greater number than the other. */
        bool operator>(const Error other) const;

        /** Compare for less-or-equal.
            \param other Other error
            \return true if this error has a lower or the same number than the other. */
        bool operator<=(const Error other) const;

        /** Compare for greater-or-equal.
            \param other Other error
            \return true if this error has a greater or the same number than the other. */
        bool operator>=(const Error other) const;

     private:
        /** Error number. */
        ErrorNumber_t m_number;
    };

} }

inline afl::sys::Error::ErrorNumber_t
afl::sys::Error::getNumber() const
{
    return m_number;
}

inline
afl::sys::Error::operator bool() const
{
    return m_number != 0;
}

inline bool
afl::sys::Error::operator==(const Error other) const
{
    return m_number == other.m_number;
}

inline bool
afl::sys::Error::operator!=(const Error other) const
{
    return m_number != other.m_number;
}

inline bool
afl::sys::Error::operator<(const Error other) const
{
    return m_number < other.m_number;
}

inline bool
afl::sys::Error::operator>(const Error other) const
{
    return m_number > other.m_number;
}

inline bool
afl::sys::Error::operator<=(const Error other) const
{
    return m_number <= other.m_number;
}

inline bool
afl::sys::Error::operator>=(const Error other) const
{
    return m_number >= other.m_number;
}

#endif
