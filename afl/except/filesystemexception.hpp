/**
  *  \file afl/except/filesystemexception.hpp
  *  \brief Class afl::except::FileSystemException
  */
#ifndef AFL_AFL_EXCEPT_FILESYSTEMEXCEPTION_HPP
#define AFL_AFL_EXCEPT_FILESYSTEMEXCEPTION_HPP

#include "afl/except/fileproblemexception.hpp"
#include "afl/sys/error.hpp"

namespace afl { namespace except {

    /** Problem with a file, reported by operating system.
        Reports a problem indicated by a failing operating system call.

        The what() string contains just the error message, as provided by afl::sys::Error::getText().
        A file name is passed in addition to that. */
    class FileSystemException : public FileProblemException {
     public:
        /** Construct from file name and error.
            \param fileName File name
            \param error Error code */
        FileSystemException(const String_t& fileName, afl::sys::Error error);

        /** Construct from stream and error.
            \param file Stream
            \param error Error code */
        FileSystemException(afl::io::Stream& file, afl::sys::Error error);

        /** Destructor. */
        ~FileSystemException() throw();

        /** Get error code.
            \return The error code this was instantiated with. */
        afl::sys::Error getError() const;

     private:
        afl::sys::Error m_error;
    };

} }

inline afl::sys::Error
afl::except::FileSystemException::getError() const
{
    return m_error;
}

#endif
