/**
  *  \file afl/io/xml/reader.hpp
  *  \brief Class afl::io::xml::Reader
  */
#ifndef AFL_AFL_IO_XML_READER_HPP
#define AFL_AFL_IO_XML_READER_HPP

#include <memory>
#include "afl/io/xml/basereader.hpp"
#include "afl/io/stream.hpp"
#include "afl/base/types.hpp"
#include "afl/charset/unicode.hpp"
#include "afl/charset/utf8.hpp"
#include "afl/charset/codepagecharset.hpp"
#include "afl/charset/charsetfactory.hpp"

namespace afl { namespace io { namespace xml {

    class EntityHandler;

    /** XML reader.
        Implements a basic XML reader that should be usable for most cases:
        - tags are returned as-is, no handling of namespaces
        - entity references are decoded
        - no DTD handling
        - supports UTF-8, UTF-16LE, UTF-16BE and 1-byte codepages, with partial auto-detection */
    class Reader : public BaseReader {
     public:
        /** Constructor.
            Note that this will already read the first bytes from the stream to detect the character encoding.
            \param stream Stream to read from
            \param entityHandler Entity handler used to expand entity references
            \param charsetFactory CharsetFactory instance

            That the \c charsetFactory is used for character sets that are explicitly declared by the XML file.
            Only single-byte character sets are supported so far:
            all results that are not a afl::charset::CodepageCharset are ignored and treated as UTF-8. */
        Reader(Stream& stream, EntityHandler& entityHandler, afl::charset::CharsetFactory& charsetFactory);

        /** Virtual destructor. */
        virtual ~Reader();

        // BaseReader methods:
        virtual Token readNext();
        virtual String_t getTag() const;
        virtual String_t getName() const;
        virtual String_t getValue() const;
        virtual void setWhitespaceMode(WhitespaceMode mode);
        virtual WhitespaceMode getWhitespaceMode() const;


        /*
         *  Reader
         */

        /** Get position of current token.
            This position can later be used to restore that position using setPos().

            <b>Note:</b> The value is guaranteed to be usable for token types TagStart and Text only.
            It is not possible to resume parsing within a tag attribute, for example.

            \return position */
        Stream::FileSize_t getPos() const;

        /** Set read position.
            \param pos Position to resume reading at; obtained by calling getPos(). */
        void setPos(Stream::FileSize_t pos);

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
        String_t m_tag;              // name of current tag
        String_t m_name;             // name of current attribute
        String_t m_value;            // value or text
        Stream::FileSize_t m_tagPos; // position of tag in file

        /*
         *  Buffering
         */
        afl::base::Bytes_t m_buffer;
        uint8_t m_bufferData[1024];
        Stream::FileSize_t m_bufferPos;

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
        std::auto_ptr<afl::charset::CodepageCharset> m_codepage;
        afl::charset::CharsetFactory& m_charsetFactory;
        afl::charset::Utf8 m_unicodeHandler;
        bool m_haveCurrentCharacter;
        afl::charset::Unichar_t m_currentCharacter;
        Stream::FileSize_t m_charPos;

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
