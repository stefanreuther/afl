/**
  *  \file afl/io/internaltextwriter.hpp
  *  \brief Class afl::io::InternalTextWriter
  */
#ifndef AFL_AFL_IO_INTERNALTEXTWRITER_HPP
#define AFL_AFL_IO_INTERNALTEXTWRITER_HPP

#include "afl/string/string.hpp"
#include "afl/io/textwriter.hpp"
#include "afl/base/growablememory.hpp"

namespace afl { namespace io {

    /** Internal (In-Memory) TextWriter.
        Implements the TextWriter interface by writing into a GrowableMemory<char>.
        Unlike a TextFile writing into an InternalStream, this class has
        - no char-to-byte conversion
        - no line-feed conversion (linefeed always is "\n")
        - no buffering (no flush required)
        and is thus much simpler. */
    class InternalTextWriter : public TextWriter {
     public:
        /** Default constructor.
            Makes empty InternalTextWriter. */
        InternalTextWriter();

        /** Destructor.
            Makes empty InternalTextWriter. */
        ~InternalTextWriter();

        /** Clear.
            Discards all content. */
        void clear();

        /** Get content.
            \return content. Valid until the next call that modifies the object. */
        afl::string::ConstStringMemory_t getContent() const;

     protected:
        virtual void doWriteText(afl::string::ConstStringMemory_t data);
        virtual void doWriteNewline();
        virtual void doFlush();

     private:
        afl::base::GrowableMemory<char> m_content;
    };

} }

#endif
