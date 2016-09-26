/**
  *  \file afl/io/xml/reader.cpp
  *  \brief Class afl::io::xml::Reader
  *
  *  \todo This class' logic comes directly from pcc-v2/util/xmlread.cc, with a few interfaces thrown in.
  *  Debatable choices include:
  *  - buffering. Do we need to implement our own buffering? So far, it's the peek() method that makes it necessary.
  *    Merging character detection into the main state machine would allow getting rid of that,
  *    as well as the I/O in the constructor.
  *  - character set handling. PCC2's version has codepage handling directly wired in.
  *    This is completely missing now.
  */

#include <cassert>
#include "afl/io/xml/reader.hpp"
#include "afl/charset/utf8.hpp"
#include "afl/io/xml/entityhandler.hpp"

namespace {
    // Options/character classes for Reader::readCharacterSequence:
    const int WhitespaceChar = 1;
    const int LTChar         = 2;
    const int GTChar         = 4;
    const int EQChar         = 8;
    const int BangChar       = 16;
    const int QuesChar       = 32;
    const int SlashChar      = 64;
    const int MinusChar      = 128;
    const int LBracketChar   = 256;
    const int AnyChar        = 512;
    const int ProcessQuotes  = 16384;
}

// Constructor.
afl::io::xml::Reader::Reader(Stream& stream, EntityHandler& entityHandler)
    : m_stream(stream),
      m_entityHandler(entityHandler),
      m_whitespaceMode(TrimWS),
      m_state(Main),
      m_tag(),
      m_name(),
      m_value(),

      m_buffer(),

      m_encoding(Utf8),
      m_unicodeHandler(),
      // codepage(),
      m_haveCurrentCharacter(false),
      m_currentCharacter(0)
{
    refillBuffer();
    detectEncoding();
    readNextChar();
}

// Virtual destructor.
afl::io::xml::Reader::~Reader()
{ }

// Read next token.
afl::io::xml::Reader::Token
afl::io::xml::Reader::readNext()
{
    while (1) {
        Token t = readNextToken();
        if (t == Null) {
            // ignore
        } else if (t == Text && m_value.empty()) {
            // ignore
        } else {
            // accept
            return t;
        }
    }
}

// Get current tag name.
String_t
afl::io::xml::Reader::getTag() const
{
    return m_tag;
}

// Get current attribute name.
String_t
afl::io::xml::Reader::getName() const
{
    return m_name;
}

// Get current node value.
String_t
afl::io::xml::Reader::getValue() const
{
    return m_value;
}

// Get whitespace mode.
afl::io::xml::Reader::WhitespaceMode
afl::io::xml::Reader::getWhitespaceMode() const
{
    return m_whitespaceMode;
}

// Set whitespace mode.
void
afl::io::xml::Reader::setWhitespaceMode(WhitespaceMode mode)
{
    m_whitespaceMode = mode;
}

/*
 *  Private Methods
 */

/** Read a (pseudo)token. */
afl::io::xml::Reader::Token
afl::io::xml::Reader::readNextToken()
{
    // End of file reached?
    if (!m_haveCurrentCharacter) {
        return Eof;
    }

    // Regular processing
    switch (m_state) {
     case Main:
        if (m_currentCharacter == '<') {
            // Tag
            readNextChar();
            readCharacterSequence(WhitespaceChar, 0);
            if (!m_haveCurrentCharacter) {
                // "<" + EOF
                return Eof;
            } else if (m_currentCharacter == '?') {
                // "<?NAME ...."
                readNextChar();
                m_tag.clear();
                readCharacterSequence(WhitespaceChar, 0);
                readCharacterSequence(~(WhitespaceChar | GTChar | QuesChar | GTChar), &m_tag);
                m_state = InPI;
                return PIStart;
            } else if (m_currentCharacter == '/') {
                // "</NAME>"
                readNextChar();
                m_tag.clear();
                readCharacterSequence(WhitespaceChar, 0);
                readCharacterSequence(~(WhitespaceChar | GTChar), &m_tag);  // read tag name
                readCharacterSequence(~GTChar, 0);                          // ignore up to closing angle bracket
                readNextChar();                                             // read closing angle bracket
                return TagEnd;
            } else if (m_currentCharacter == '!') {
                // "<!-- .... -->" or "<![CDATA[....]]>"
                readNextChar();
                if (!m_haveCurrentCharacter) {
                    return Eof;
                } else if (m_currentCharacter == '[') {
                    String_t tmp;
                    readNextChar();
                    readCharacterSequence(~(LBracketChar | ProcessQuotes), &tmp);
                    if (tmp == "CDATA") {
                        // "<![CDATA[....]]>"
                        readNextChar();                                     // skip the final "["
                        m_value.clear();
                        readUnparsedCharacterData(m_value);
                        return Text;
                    } else {
                        // Any other directive (INCLUDE and IGNORE can appear in DTDs)
                        readCharacterSequence(~GTChar, 0);
                        readNextChar();
                        return Error;
                    }
                } else if (m_currentCharacter == '-') {
                    // possible comment
                    if (readCharacterSequence(MinusChar, 0) >= 2) {
                        // "<!--". Comment ends at "-->". XML-1.1 forbids "--" within a comment,
                        // so we treat that as the comment end (although we allow more than two
                        // "--"s, which is also not permitted).
                        do {
                            readCharacterSequence(~(MinusChar | ProcessQuotes), 0);
                        } while (m_currentCharacter == '-' && readCharacterSequence(MinusChar, 0) < 2);
                    }
                    readCharacterSequence(~GTChar, 0);
                    readNextChar();
                    return Comment;
                } else {
                    // anything else, e.g. "<!DOCTYPE>" block: skip until '>'
                    readCharacterSequence(~GTChar, 0);
                    readNextChar();
                    return Null;
                }
            } else {
                // "<NAME....>"
                m_tag.clear();
                readCharacterSequence(~(WhitespaceChar | GTChar | SlashChar), &m_tag);    // read tag name
                m_state = InTag;
                return TagStart;
            }
        } else {
            // Text
            m_value.clear();
            readCharacterSequence(~(LTChar | ProcessQuotes), &m_value);
            if (m_whitespaceMode != AllWS && m_value.find_first_not_of("\r\n\t ") == m_value.npos) {
                if (m_whitespaceMode == SingleWS) {
                    m_value = " ";
                } else {
                    m_value.clear();
                }
            } else {
                m_value = expandEntities(m_value);
            }
            return Text;
        }
        break;

     case InTag:
        readCharacterSequence(WhitespaceChar, 0);
        if (!m_haveCurrentCharacter) {
            // Document ends in tag
            return Eof;
        } else if (m_currentCharacter == '/') {
            // Tag ends and is immediately closed
            readCharacterSequence(~GTChar, 0);
            readNextChar();
            m_state = Main;
            return TagEnd;
        } else if (m_currentCharacter == '>') {
            // Tag ends normally
            readNextChar();
            m_state = Main;
            return Null;
        } else {
            // Tag attribute
            m_name.clear();
            readCharacterSequence(~(WhitespaceChar | EQChar | GTChar | SlashChar), &m_name);
            readCharacterSequence(WhitespaceChar, 0);
            if (!m_haveCurrentCharacter) {
                // Document ends before attribute starts
                return Eof;
            } else if (m_currentCharacter == '=') {
                readNextChar();
                m_value.clear();
                readCharacterSequence(WhitespaceChar, 0);
                readCharacterSequence(~(WhitespaceChar | GTChar| SlashChar), &m_value);
                m_value = expandEntities(m_value);
                return TagAttribute;
            } else {
                m_value = m_name;
                return TagAttribute;
            }
        }

     case InPI:
        readCharacterSequence(WhitespaceChar, 0);
        if (!m_haveCurrentCharacter) {
            return Eof;
        } else if (m_currentCharacter == '?' || m_currentCharacter == '>') {
            // PI ends
            readCharacterSequence(~GTChar, 0);
            readNextChar();
            m_state = Main;
            return Null;
        } else {
            // PI attribute
            m_name.clear();
            readCharacterSequence(~(WhitespaceChar | EQChar | QuesChar | GTChar), &m_name);
            readCharacterSequence(WhitespaceChar, 0);
            if (!m_haveCurrentCharacter) {
                return Eof;
            } else {
                if (m_currentCharacter == '=') {
                    readNextChar();
                    m_value.clear();
                    readCharacterSequence(WhitespaceChar, 0);
                    readCharacterSequence(~(WhitespaceChar | QuesChar | GTChar), &m_value);
                } else {
                    m_value = m_name;
                }

                // Check for encoding attribute. Only when in UTF-8 or codepage mode (that is, we are not
                // already interpreting this as a 16-bits-per-character file, and have not
                // yet seen another encoding name).
                if ((m_encoding == Utf8 || m_encoding == Codepage) && m_tag == "xml" && m_name == "encoding") {
                    // if (CharacterSet::isCharsetNameUtf8(m_value)) {
                    //     // UTF-8.
                    m_encoding = Utf8;
                    // } else {
                    //     // Codepage. Unsupported codepages are mapped to default (=latin1).
                    //     m_encoding = Codepage;
                    //     codepage = CharacterSet::getCharsetByName(m_value);
                    // }
                }

                return PIAttribute;
            }
        }
    }

    return Error;
}

/** Read next character.
    Reads a Unicode character into m_currentCharacter.
    Handles all encodings. */
void
afl::io::xml::Reader::readNextChar()
{
    uint8_t a, b, c;
    switch (m_encoding) {
     case Codepage:
        // Right now, fall through to UTF-8 case; we'll do codepages later
        // a = readByte();
        // if (a >= 0) {
        //     // Valid character
        //     m_currentCharacter = codepage.getUnicode(a);
        // } else {
        //     // End of file
        //     m_currentCharacter = a;
        // }
        // break;

     case Utf8:
        if (!readByte(a)) {
            m_haveCurrentCharacter = false;
        } else if (a >= 0xC0 && a < 0xE0) {
            // two-byte rune
            if (!readByte(b)) {
                m_haveCurrentCharacter = false;
            } else {
                m_currentCharacter = ((a & 0x1F) << 6) + (b & 0x3F);
                m_haveCurrentCharacter = true;
            }
        } else if (a >= 0xE0 && a < 0xF0) {
            // three-byte rune
            if (!readByte(b) || !readByte(c)) {
                m_haveCurrentCharacter = false;
            } else {
                m_currentCharacter = ((a & 0x0F) << 12) + ((b & 0x3F) << 6) + (c & 0x3F);
                m_haveCurrentCharacter = true;
            }
        } else {
            // return as-is. This includes ASCII and invalid characters.
            m_currentCharacter = a;
            m_haveCurrentCharacter = true;
        }
        break;

     case Utf16BE:
        if (readByte(a) && readByte(b)) {
            m_currentCharacter = 256*a + b;
            m_haveCurrentCharacter = true;
        } else {
            m_haveCurrentCharacter = false;
        }
        break;

     case Utf16LE:
        if (readByte(a) && readByte(b)) {
            m_currentCharacter = 256*b + a;
            m_haveCurrentCharacter = true;
        } else {
            m_haveCurrentCharacter = false;
        }
        break;
    }
}

/** Read byte.
    \param out [out] byte goes here
    \retval true byte successfully read
    \retval false end of file */
bool
afl::io::xml::Reader::readByte(uint8_t& out)
{
    const uint8_t* pc = m_buffer.eat();
    if (pc == 0) {
        refillBuffer();
        pc = m_buffer.eat();
    }
    if (pc == 0) {
        return false;
    } else {
        out = *pc;
        return true;
    }
}

/** Fill read buffer. */
void
afl::io::xml::Reader::refillBuffer()
{
    assert(m_buffer.empty());
    m_buffer = m_bufferData;
    m_buffer.trim(m_stream.read(m_buffer));
}

/** Detect encoding.
    Assumes pre-filled buffer, otherwise it may not detect everything possible. */
void
afl::io::xml::Reader::detectEncoding()
{
    /* Encoding detection patterns according to W3C XML-1.1 document:
         00 00 FE FF   UCS-4, big-endian machine (1234 order)
         FF FE 00 00   UCS-4, little-endian machine (4321 order)
         00 00 FF FE   UCS-4, unusual octet order (2143)
         FE FF 00 00   UCS-4, unusual octet order (3412)
         FE FF         UTF-16, big-endian
         FF FE         UTF-16, little-endian
         EF BB BF      UTF-8

         00 00 00 3C   UCS-4 BE (1234)
         3C 00 00 00   UCS-4 LE (4321)
         00 00 3C 00   UCS-4 unusual (2143)
         00 3C 00 00   UCS-4 unusual (3412)
         00 3C 00 3F   UTF-16 BE
         3C 00 3F 00   UTF-16 LE
         3C 3F 78 6D   UTF-8, ISO 646, ASCII, etc.

       We want to support UTF-16 and UTF-8, so we remain at
         hh ll         UTF-16 BE
         ll hh         UTF-16 LE
         EF BB BF      UTF-8
       with hhll being a valid Unicode character, in particular 0xFEFF or,
       for simplicity, any ASCII character (including 0x3C, but also newline
       and spaces). */
    const uint8_t* p0 = m_buffer.at(0);
    const uint8_t* p1 = m_buffer.at(1);
    const uint8_t* p2 = m_buffer.at(2);
    if (p0 && p1) {
        if (p2 && *p0 == 0xEF && *p1 == 0xBB && *p2 == 0xBF) {
            // UTF-8 BOM
            m_encoding = Utf8;
            m_buffer.split(3);
        } else if (*p0 == 0xFE && *p1 == 0xFF) {
            // UTF-16BE BOM
            m_encoding = Utf16BE;
            m_buffer.split(2);
        } else if (*p0 == 0xFF && *p1 == 0xFE) {
            // UTF-16LE BOM
            m_encoding = Utf16LE;
            m_buffer.split(2);
        } else if (*p0 == 0 && *p1 != 0) {
            // UTF-16BE-encoded ASCII character
            m_encoding = Utf16BE;
        } else if (*p0 != 0 && *p1 == 0) {
            // UTF-16LE-encoded ASCII character
            m_encoding = Utf16LE;
        } else {
            // anything else
            m_encoding = Utf8;
        }
    } else {
        // anything else, short document
        m_encoding = Utf8;
    }
}

/** Read character sequence.
    \param cl     [in] Character classes to accept, plus optional options.
    \param accum  [out,optional] Characters will be stored here
    \return Number of characters read */
size_t
afl::io::xml::Reader::readCharacterSequence(int acceptClasses, String_t* accum)
{
    size_t count = 0;
    afl::charset::Unichar_t currentQuote = 0;
    while (m_haveCurrentCharacter) {
        // Figure out this character's class
        int myClass;
        if (m_currentCharacter <= ' ' || m_currentCharacter == 0x85 /* NEXT LINE */ || m_currentCharacter == 0x2028 /* LINE SEPARATOR */) {
            myClass = WhitespaceChar;
        } else if (m_currentCharacter == '<') {
            myClass = LTChar;
        } else if (m_currentCharacter == '>') {
            myClass = GTChar;
        } else if (m_currentCharacter == '=') {
            myClass = EQChar;
        } else if (m_currentCharacter == '!') {
            myClass = BangChar;
        } else if (m_currentCharacter == '?') {
            myClass = QuesChar;
        } else if (m_currentCharacter == '/') {
            myClass = SlashChar;
        } else if (m_currentCharacter == '-') {
            myClass = MinusChar;
        } else if (m_currentCharacter == '[') {
            myClass = LBracketChar;
        } else {
            myClass = AnyChar;
        }

        // Quoting
        if (currentQuote != 0 && m_currentCharacter == currentQuote) {
            // Quoting ends
            currentQuote = 0;
        } else if ((acceptClasses & ProcessQuotes) != 0 && currentQuote == 0 && (m_currentCharacter == '\"' || m_currentCharacter == '\'')) {
            // Quoting starts
            currentQuote = m_currentCharacter;
        } else if ((acceptClasses & myClass) != 0 || currentQuote != 0) {
            // Matching class or quoted
            if (accum) {
                m_unicodeHandler.append(*accum, m_currentCharacter);
            }
            ++count;
        } else {
            // Non-matching class
            break;
        }

        // Consume this character
        readNextChar();
    }
    return count;
}

/** Read content of a "<![CDATA" block.
    Assumes everything up to and including the opening bracket has already been read.
    Reads everything up to and including the final "]]>".
    \param out [out] Content of block */
void
afl::io::xml::Reader::readUnparsedCharacterData(String_t& out)
{
    // Parse content
    int numRBrack = 0;
    while (m_haveCurrentCharacter) {
        if (numRBrack == 0) {
            if (m_currentCharacter == ']') {
                // First terminator
                numRBrack = 1;
            } else {
                m_unicodeHandler.append(out, m_currentCharacter);
            }
        } else if (numRBrack == 1) {
            if (m_currentCharacter == ']') {
                // Second terminator
                numRBrack = 2;
            } else {
                // Single "]" does not terminate: append it, and append next character
                out += ']';
                numRBrack = 0;
                m_unicodeHandler.append(out, m_currentCharacter);
            }
        } else if (numRBrack == 2) {
            if (m_currentCharacter == '>') {
                // Third terminator
                readNextChar();
                break;
            } else if (m_currentCharacter == ']') {
                // Third "]" means at least one of them belongs to the text
                out += ']';
            } else {
                // Third terminator missing
                out += "]]";
                numRBrack = 0;
                m_unicodeHandler.append(out, m_currentCharacter);
            }
        }
        readNextChar();
    }
}

/** Expand entity-references within a string.
    This function uses the provided EntityHandler.
    \param s the string
    \return Updated string */
String_t
afl::io::xml::Reader::expandEntities(const String_t s)
{
    String_t out;
    String_t::size_type pos = 0;
    String_t::size_type n, q;
    while ((n = s.find('&', pos)) != String_t::npos) {
        // Locate end of entity reference
        q = s.find(';', n);
        if (q == String_t::npos)
            break;

        out.append(s, pos, n-pos);
        out.append(m_entityHandler.expandEntityReference(String_t(s, n+1, q-n-1)));
        pos = q+1;
    }
    out.append(s.substr(pos));
    return out;
}
