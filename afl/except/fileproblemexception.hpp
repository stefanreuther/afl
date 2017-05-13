/**
  *  \file afl/except/fileproblemexception.hpp
  *  \brief Class afl::except::FileProblemException
  */
#ifndef AFL_AFL_EXCEPT_FILEPROBLEMEXCEPTION_HPP
#define AFL_AFL_EXCEPT_FILEPROBLEMEXCEPTION_HPP

#include <stdexcept>
#include "afl/io/stream.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace except {

    /** Problem with a file.
        This is the base class for all file-related problems.

        It contains a message and a file name.
        The what() string contains just the error message ("invalid file header").
        A file name is passed in addition to that. */
    class FileProblemException : public std::runtime_error {
     public:
        /** Construct from file name and message.
            \param fileName File name
            \param msg Message, not ending in a period or newline */
        FileProblemException(const String_t& fileName, const String_t& msg);

        /** Construct from stream and message.
            \param file Stream
            \param msg Message, not ending in a period or newline */
        FileProblemException(afl::io::Stream& file, const String_t& msg);

        /** Destructor. */
        ~FileProblemException() throw();

        /** Get file name.
            \return File name */
        const String_t& getFileName() const;

        /** Set file name.
            \param fileName File name */
        void setFileName(const String_t& fileName);

     private:
        /** File name. */
        String_t m_fileName;
    };
    
} }

inline const String_t&
afl::except::FileProblemException::getFileName() const
{
    return m_fileName;
}

#endif
