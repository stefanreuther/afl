/**
  *  \file afl/io/textfile.hpp
  *  \brief Class afl::io::TextFile
  */
#ifndef AFL_AFL_IO_TEXTFILE_HPP
#define AFL_AFL_IO_TEXTFILE_HPP

#include <memory>
#include "afl/io/bufferedstream.hpp"
#include "afl/charset/charset.hpp"
#include "afl/string/string.hpp"
#include "afl/io/textreader.hpp"
#include "afl/io/textwriter.hpp"

namespace afl { namespace io {

    /** Buffered Text File Access.
        This class provides buffered access to a stream,
        mainly intended for text files, but also usable for binaries.

        It provides
        - character set translation for text reading and writing
        - accepts all usual text file conventions; writes text file convention of host system
        - optional UTF-8 BOM detection
        - binary read and write
        - binary and text accesses can be mixed freely
        - read, write, and setPos can be mixed freely

        TextReader functions convert input from the file character set to UTF-8.
        TextWriter functions convert text from UTF-8 to the file character set before writing it out.

        Note that written data is written to BufferedStream's buffer first,
        and exceptions are not reported until the buffer is actually written out. */
    class TextFile : public BufferedStream, public TextReader, public TextWriter {
     public:
        /** Constructor.
            \param s Stream to read from/write to. */
        TextFile(Stream& s);

        /** Destructor. */
        ~TextFile();


        /*
         *  Text Format
         */

        /** Set character set.
            The default character set is Latin-1.
            Explicitly setting a character set turns off UTF-8 detection.
            \param cs Newly-allocated character coder. TextFile takes ownership. Must not be null. */
        void setCharsetNew(afl::charset::Charset* cs);

        /** Set newline format.
            \param flag Format flag. When true, uses the system's native newlines.
                        When false, writes just a single "\n". */
        void setSystemNewline(bool flag);

        /** Set UTF-8 detection.
            The default is enabled; if a UTF-8 BOM is found, the character translation is automatically set to UTF-8.
            The detector automatically turns off after a line has been written
            which may or may not have contained the BOM.
            You can re-enable it for further lines.
            \param flag Flag. true enables the detector, false disables it. */
        void setUtf8Snoop(bool flag);

        // We prefer BufferedStream's flush method.
        using BufferedStream::flush;

     protected:
        // TextReader:
        virtual bool doReadLine(String_t& line);

        // TextWriter:
        virtual void doWriteText(afl::string::ConstStringMemory_t data);
        virtual void doWriteNewline();
        virtual void doFlush();

     private:
        std::auto_ptr<afl::charset::Charset> m_charset;
        bool m_utf8Snoop;
        bool m_sysNewline;
    };

} }

#endif
