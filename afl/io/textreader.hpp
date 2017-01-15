/**
  *  \file afl/io/textreader.hpp
  *  \brief Interface afl::io::TextReader
  */
#ifndef AFL_AFL_IO_TEXTREADER_HPP
#define AFL_AFL_IO_TEXTREADER_HPP

#include "afl/base/types.hpp"
#include "afl/string/string.hpp"
#include "afl/base/refcounted.hpp"

namespace afl { namespace io {

    /** Interface for reading text, line-by-line. */
    class TextReader : public afl::base::RefCounted {
     public:
        /** Line number. */
        typedef int32_t LineNumber_t;

        /** Constructor. */
        TextReader();

        /** Virtual destructor. */
        virtual ~TextReader();

        
        /*
         *  Reading
         */

        /** Read a line of text.
            Reads the line and an optional line separator character.
            \param line [out] line in UTF-8
            \retval true Line was read
            \retval false No line was read because the stream ended; line is the empty string */
        bool readLine(String_t& line);

        /** Get current line number.
            \return 1-based number of the line last read with readLine() */
        LineNumber_t getLineNumber() const;

     protected:
        /** Implementation of readLine(): read a line of text.
            Reads the line and an optional line separator character.
            \param line [out] line in UTF-8
            \retval true Line was read
            \retval false No line was read because the stream ended; line is the empty string */
        virtual bool doReadLine(String_t& out) = 0;

     private:
        LineNumber_t m_lineNumber;
    };

} }

#endif
