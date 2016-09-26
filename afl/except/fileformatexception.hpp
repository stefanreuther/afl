/**
  *  \file afl/except/fileformatexception.hpp
  *  \brief Class afl::except::FileFormatException
  */
#ifndef AFL_AFL_EXCEPT_FILEFORMATEXCEPTION_HPP
#define AFL_AFL_EXCEPT_FILEFORMATEXCEPTION_HPP

#include "afl/except/fileproblemexception.hpp"

namespace afl { namespace except {

    /** Problem with a file format.
        Reports a problem about a file, that lies in the file itself
        (i.e. not a data transmission problem).

        The what() string contains just the error message ("invalid file header").
        A file name is passed in addition to that. */
    class FileFormatException : public FileProblemException {
     public:
        /** Construct from file name and message.
            \param fileName File name
            \param msg Message, not ending in a period or newline */
        FileFormatException(const String_t& fileName, const String_t& msg);

        /** Construct from stream and message.
            \param file Stream
            \param msg Message, not ending in a period or newline */
        FileFormatException(afl::io::Stream& file, const String_t& msg);

        /** Destructor. */
        ~FileFormatException() throw();
    };

} }

#endif
