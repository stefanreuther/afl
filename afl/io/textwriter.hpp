/**
  *  \file afl/io/textwriter.hpp
  *  \brief Interface afl::io::TextWriter
  */
#ifndef AFL_AFL_IO_TEXTWRITER_HPP
#define AFL_AFL_IO_TEXTWRITER_HPP

#include "afl/string/string.hpp"
#include "afl/base/refcounted.hpp"

namespace afl { namespace io {

    /** Interface for writing text. */
    class TextWriter : public afl::base::RefCounted {
     public:
        virtual ~TextWriter();

        /** Write text.
            Writes text to the output device.
            Does not append a line separator; see writeLine for that.
            Embedded newlines are converted to proper output newlines.
            \param text [in] line in UTF-8
            \throw FileProblemException if data cannot be written. */
        void writeText(const String_t& text);

        /** Write text.
            Writes text to the output device.
            Does not append a line separator; see writeLine for that.
            Embedded newlines are converted to proper output newlines.
            \param text [in] line in UTF-8
            \throw FileProblemException if data cannot be written. */
        void writeText(const char* text);

        /** Write text.
            Writes text to the output device.
            Does not append a line separator; see writeLine for that.
            Embedded newlines are converted to proper output newlines.
            \param data [in] line in UTF-8
            \throw FileProblemException if data cannot be written. */
        void writeText(afl::string::ConstStringMemory_t data);

        /** Write a line of text.
            Writes text to the output device, together with a line separator.
            \param text [in] line in UTF-8
            \throw FileProblemException if data cannot be written. */
        void writeLine(const String_t& text);

        /** Write a line separator.
            \throw FileProblemException if data cannot be written. */
        void writeLine();

        /** Force output out.
            \throw FileProblemException if data cannot be written. */
        void flush();

     protected:
        /** Write text. This implements writeText().
            This function is not required to handle embedded "\n",
            this is done by the caller.
            \param data Text to write, UTF-8 */
        virtual void doWriteText(afl::string::ConstStringMemory_t data) = 0;

        /** Write newline. This implements writeLine(). */
        virtual void doWriteNewline() = 0;

        /** Force output out. This implements flush(). */
        virtual void doFlush() = 0;
    };

} }

#endif
