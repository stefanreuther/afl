/**
  *  \file afl/io/xml/reader.hpp
  *  \brief Class afl::io::xml::Reader
  */
#ifndef AFL_AFL_IO_XML_READER_HPP
#define AFL_AFL_IO_XML_READER_HPP

#include "afl/io/xml/basereader.hpp"
#include "afl/io/stream.hpp"
#include "afl/base/types.hpp"
#include "afl/charset/unicode.hpp"
#include "afl/charset/utf8.hpp"

namespace afl { namespace io { namespace xml {

    class EntityHandler;

    /** XML reader.
        Implements a basic XML reader that should be usable for most cases:
        - tags are returned as-is, no handling of namespaces
        - entity references are decoded
        - no DTD handling
        - supports UTF-8, UTF-16LE, UTF-16BE encodings and auto-detection */
    class Reader : public BaseReader {
     public:
        /** Constructor.
            Note that this will already read the first bytes from the stream to detect the character encoding.
            \param stream Stream to read from
            \param entityHandler Entity handler used to expand entity references */
        Reader(Stream& stream, EntityHandler& entityHandler);

        /** Virtual destructor. */
        virtual ~Reader();

        // BaseReader methods:
        virtual Token readNext();
        virtual String_t getTag() const;
        virtual String_t getName() const;
        virtual String_t getValue() const;
        virtual void setWhitespaceMode(WhitespaceMode mode);
        virtual WhitespaceMode getWhitespaceMode() const;

     private:
        /*
         *  Configuration
         */
        Stream& m_stream;
        EntityHandler& m_entityHandler;
        WhitespaceMode m_whitespaceMode;

        /*
         *  State
         */
        enum State {
            Main,                   // regular state
            InTag,                  // after reading an opening tag
            InPI                    // after reading an opening PI
        };
        State m_state;
        String_t m_tag;           // name of current tag
        String_t m_name;          // name of current attribute
        String_t m_value;         // value or text

        /*
         *  Buffering
         */
        afl::base::Bytes_t m_buffer;
        uint8_t m_bufferData[1024];

        /*
         *  Character handling
         */
        enum Encoding {
            Utf8,
            Codepage,
            Utf16LE,
            Utf16BE
        };
        Encoding m_encoding;
        // CharacterSet codepage;
        afl::charset::Utf8 m_unicodeHandler;
        bool m_haveCurrentCharacter;
        afl::charset::Unichar_t m_currentCharacter;

        // Private methods:
        Token readNextToken();
        void readNextChar();
        bool readByte(uint8_t& out);
        void refillBuffer();
        void detectEncoding();
        size_t readCharacterSequence(int cl, String_t* accum);
        void readUnparsedCharacterData(String_t& out);
        String_t expandEntities(String_t s);
    };

} } }

#endif
