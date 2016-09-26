/**
  *  \file afl/except/remoteerrorexception.hpp
  *  \brief Class afl::except::RemoteErrorException
  */
#ifndef AFL_AFL_EXCEPT_REMOTEERROREXCEPTION_HPP
#define AFL_AFL_EXCEPT_REMOTEERROREXCEPTION_HPP

#include "afl/except/fileproblemexception.hpp"

namespace afl { namespace except {

    /** Problem signaled by remote side.
        This is not an error with the data, or a data transfer error,
        but the data contains a signal of an error.
        The message is typically provided by the remote side (but must be normalized to UTF-8). */
    // FIXME: should this be a FileProblemException? Probably not.
    class RemoteErrorException : public FileProblemException {
     public:
        /** Construct from file name and message.
            \param fileName File name
            \param msg Message, not ending in a period or newline */
        RemoteErrorException(const String_t& fileName, const String_t& msg);

        /** Construct from stream and message.
            \param file Stream
            \param msg Message, not ending in a period or newline */
        RemoteErrorException(afl::io::Stream& file, const String_t& msg);

        /** Destructor. */
        ~RemoteErrorException() throw();
    };

} }

#endif
