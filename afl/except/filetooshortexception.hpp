/**
  *  \file afl/except/filetooshortexception.hpp
  *  \brief Class afl::except::FileTooShortException
  */
#ifndef AFL_AFL_EXCEPT_FILETOOSHORTEXCEPTION_HPP
#define AFL_AFL_EXCEPT_FILETOOSHORTEXCEPTION_HPP

#include "afl/except/fileformatexception.hpp"

namespace afl { namespace except {

    /** Problem with a file that is too short.
        Reports a problem about a file that is too short.
        This is a special case of a file format problem, in its own class because it's common.

        The what() string contains just the error message (fixed for FileTooShortException).
        A file name is passed in addition to that. */
    class FileTooShortException : public FileFormatException {
     public:
        /** Construct from file name and message.
            \param fileName File name */
        FileTooShortException(const String_t& fileName);

        /** Construct from stream.
            \param file Stream */
        FileTooShortException(afl::io::Stream& file);

        /** Destructor. */
        ~FileTooShortException() throw();
    };

} }

#endif
