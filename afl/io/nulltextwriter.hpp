/**
  *  \file afl/io/nulltextwriter.hpp
  *  \brief Class afl::io::NullTextWriter
  */
#ifndef AFL_AFL_IO_NULLTEXTWRITER_HPP
#define AFL_AFL_IO_NULLTEXTWRITER_HPP

#include "afl/io/textwriter.hpp"

namespace afl { namespace io {

    /** A TextWriter that discards all output. */
    class NullTextWriter : public TextWriter {
     protected:
        /** Write text. This implements writeText().
            This function is not required to handle embedded "\n".
            \param data Text to write, UTF-8 */
        virtual void doWriteText(afl::string::ConstStringMemory_t data);

        /** Write newline. This implements writeLine(). */
        virtual void doWriteNewline();

        /** Force output out. This implements flush(). */
        virtual void doFlush();
    };

} }

#endif
