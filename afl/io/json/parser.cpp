/**
  *  \file afl/io/json/parser.cpp
  *  \brief Class afl::io::json::Parser
  *
  *  Normative references:
  *  - RfC 7159: https://tools.ietf.org/rfc/rfc7159.txt
  *    (specifies a JSON object to have any data type)
  *  - RfC 4627: https://tools.ietf.org/rfc/rfc4627.txt
  *    (specifies a JSON object to be an array or hash)
  *  - http://json.org/
  *    (specifies a JSON object to have any data type)
  *
  *  \todo There is no character set handling yet.
  *  Input is always assumed to be UTF-8.
  *  CESU-8 ("\u" quoted or not) and invalid UTF-8 remain this way in the parsed objects.
  */

#include <memory>
#include "afl/io/json/parser.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/namemap.hpp"
#include "afl/charset/utf8.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/parse.hpp"


afl::io::json::Parser::Parser(afl::io::BufferedStream& stream, afl::data::ValueFactory& factory)
    : m_stream(stream),
      m_factory(factory)
{ }

afl::io::json::Parser::~Parser()
{ }

afl::data::Value*
afl::io::json::Parser::parse()
{
    // Skip initial whitespace
    skipWhitespace();

    // Determine item
    //   value = false / null / true / object / array / number / string
    const uint8_t* ch = m_stream.peekByte();
    if (ch == 0) {
        endOfFile();
    }

    if (*ch == '[') {
        // array
        return parseArray();
    } else if (*ch == '{') {
        // object
        return parseHash();
    } else if (*ch == '"') {
        // string
        return m_factory.createString(parseString());
    } else if (*ch == 'n') {
        // null  = %x6e.75.6c.6c      ; null
        parseKeyword("null");
        return m_factory.createNull();
    } else if (*ch == 't') {
        // true  = %x74.72.75.65      ; true
        parseKeyword("true");
        return m_factory.createBoolean(true);
    } else if (*ch == 'f') {
        // false = %x66.61.6c.73.65   ; false
        parseKeyword("false");
        return m_factory.createBoolean(false);
    } else if (*ch == '-' || (*ch >= '0' && *ch <= '9')) {
        // number
        return parseNumber();
    } else {
        // invalid
        syntaxError();
        return 0;
    }
}

afl::data::Value*
afl::io::json::Parser::parseComplete()
{
    std::auto_ptr<afl::data::Value> result(parse());
    skipWhitespace();
    if (m_stream.peekByte() != 0) {
        syntaxError();
    }
    return result.release();
}

void
afl::io::json::Parser::skipWhitespace()
{
    //   ws = *(
    //           %x20 /              ; Space
    //           %x09 /              ; Horizontal tab
    //           %x0A /              ; Line feed or New line
    //           %x0D )              ; Carriage return
    while (const uint8_t* ch = m_stream.peekByte()) {
        if (*ch != ' ' && *ch != '\t' && *ch != '\r' && *ch != '\n') {
            break;
        }
        m_stream.readByte();
    }
}

/** Internal - Throw an End of File exception. */
void
afl::io::json::Parser::endOfFile()
{
    throw afl::except::FileTooShortException(m_stream);
}

/** Internal - Throw a Syntax Error exception. */
void
afl::io::json::Parser::syntaxError()
{
    throw afl::except::FileFormatException(m_stream, afl::string::Messages::syntaxError());
}

/** Internal - Parse an Array.
    Upon entry, read pointer points at the "[".
    Upon exit, read pointer points after the "]". */
afl::data::Value*
afl::io::json::Parser::parseArray()
{
    //   begin-array     = ws %x5B ws  ; [ left square bracket
    //   value-separator = ws %x2C ws  ; , comma
    //   end-array       = ws %x5D ws  ; ] right square bracket
    //   array = begin-array [ value *( value-separator value ) ] end-array

    // Skip the opening bracket
    m_stream.readByte();
    skipWhitespace();

    // Do we have any content?
    const uint8_t* ch = m_stream.peekByte();
    if (ch == 0) {
        endOfFile();
    }

    afl::data::Segment values;
    if (*ch != ']') {
        // Read content
        while (1) {
            values.pushBackNew(parse());
            skipWhitespace();

            if (parseChar(',', ']')) {
                // comma
                m_stream.readByte();
            } else {
                // closing bracket
                break;
            }
        }
    }

    // Skip the closing bracket
    m_stream.readByte();

    // Produce result
    return m_factory.createVector(values);
}

/** Internal - Parse a Hash.
    Upon entry, read pointer points at the "{".
    Upon exit, read pointer points after the "}". */
afl::data::Value*
afl::io::json::Parser::parseHash()
{
    //   begin-object    = ws %x7B ws  ; { left curly bracket
    //   end-object      = ws %x7D ws  ; } right curly bracket
    //   name-separator  = ws %x3A ws  ; : colon
    //   value-separator = ws %x2C ws  ; , comma
    //   object = begin-object [ member *( value-separator member ) ]
    //            end-object
    //   member = string name-separator value

    // Skip the opening brace
    m_stream.readByte();
    skipWhitespace();

    // Do we have any content?
    const uint8_t* ch = m_stream.peekByte();
    if (ch == 0) {
        endOfFile();
    }

    afl::data::Segment values;
    afl::data::NameMap names;
    if (*ch != '}') {
        // Read content
        while (1) {
            // Member name
            // Note: if this finds a duplicate key, it will replace the existing value.
            // RfC 4627 says there SHOULD not be duplicate keys, which some authors seem to interpret as a permission to do it anyway.
            // Reliably picking the last element seems to be a good compromise: https://esdiscuss.org/topic/json-duplicate-keys
            // Picking a different element across different JSON parsers can be a security problem
            // (https://justi.cz/security/2017/11/14/couchdb-rce-npm.html).
            parseChar('"');
            afl::data::NameMap::Index_t index = names.addMaybe(parseString());

            // Delimiter
            skipWhitespace();
            parseChar(':');
            m_stream.readByte();

            // Value
            values.setNew(index, parse());

            // Next element
            skipWhitespace();
            if (parseChar(',', '}')) {
                // Comma
                m_stream.readByte();
                skipWhitespace();
            } else {
                // Brace
                break;
            }
        }
    }

    // Skip the closing brace
    m_stream.readByte();

    // Produce result
    return m_factory.createHash(names, values);
}

/** Internal - Parse a Number.
    Upon entry, read pointer points at the first digit.
    Upon exit, read pointer points after the last character making up the number. */
afl::data::Value*
afl::io::json::Parser::parseNumber()
{
    //   number = [ minus ] int [ frac ] [ exp ]
    //   decimal-point = %x2E       ; .
    //   digit1-9 = %x31-39         ; 1-9
    //   e = %x65 / %x45            ; e E
    //   exp = e [ minus / plus ] 1*DIGIT
    //   frac = decimal-point 1*DIGIT
    //   int = zero / ( digit1-9 *DIGIT )
    //   minus = %x2D               ; -
    //   plus = %x2B                ; +
    //   zero = %x30                ; 0

    // Let's make it simple: just collect characters; then let libc do the job.
    String_t str;
    const uint8_t* ch;
    while ((ch = m_stream.peekByte()) != 0
           && (*ch == '+' || *ch == '-' || *ch == '.' || *ch == 'e' || *ch == 'E'
               || (*ch >= '0' && *ch <= '9')))
    {
        str.append(1, char(*ch));
        m_stream.readByte();
    }

    // Parse it
    int32_t iv;
    if (afl::string::strToInteger(str, iv)) {
        // Valid integer
        return m_factory.createInteger(iv);
    }

    double fv;
    if (afl::string::strToFloat(str, fv)) {
        // Valid float
        return m_factory.createFloat(fv);
    }

    // Invalid
    syntaxError();
    return 0;
}

/** Internal - Parse a string.
    Upon entry, read pointer points at the opening quote.
    Upon exit, read pointer points after the closing quote.
    \return string, not a StringValue! */
String_t
afl::io::json::Parser::parseString()
{
    //   string = quotation-mark *char quotation-mark
    //   char = unescaped /
    //       escape (
    //           %x22 /          ; "    quotation mark  U+0022
    //           %x5C /          ; \    reverse solidus U+005C
    //           %x2F /          ; /    solidus         U+002F
    //           %x62 /          ; b    backspace       U+0008
    //           %x66 /          ; f    form feed       U+000C
    //           %x6E /          ; n    line feed       U+000A
    //           %x72 /          ; r    carriage return U+000D
    //           %x74 /          ; t    tab             U+0009
    //           %x75 4HEXDIG )  ; uXXXX                U+XXXX
    //   escape = %x5C              ; backslash
    //   quotation-mark = %x22      ; "
    //   unescaped = %x20-21 / %x23-5B / %x5D-10FFFF

    // Skip opening quotation mark
    m_stream.readByte();

    String_t value;
    while (1) {
        // Read character
        const uint8_t* ch = m_stream.readByte();
        if (ch == 0) {
            endOfFile();
        }

        if (*ch == '"') {
            // End of string
            break;
        } else if (*ch == '\\') {
            // Quote
            ch = m_stream.readByte();
            if (ch == 0) {
                endOfFile();
            }
            switch (*ch) {
             case '"':  value.append(1, '"');  break;
             case '\'': value.append(1, '\''); break; // nonstandard but appears occasionally
             case '\\': value.append(1, '\\'); break;
             case '/':  value.append(1, '/');  break;
             case 'b':  value.append(1, '\b'); break;
             case 'f':  value.append(1, '\f'); break;
             case 'n':  value.append(1, '\n'); break;
             case 'r':  value.append(1, '\r'); break;
             case 't':  value.append(1, '\t'); break;
             case 'u': afl::charset::Utf8(0).append(value, parseUnicodeEscape()); break;
             default:
                syntaxError();
                break;
            }
        } else if (*ch < 0x20) {
            // Invalid
            syntaxError();
        } else {
            // Literal
            value.append(1, char(*ch));
        }
    }

    return value;
}

/** Internal - Expect a character.
    Makes sure that we are about to read the character specified in \c which (but does not consume it).
    If this assumption fails, throws an appropriate exception (syntaxError, endOfFile).
    \param which Character to expect */
void
afl::io::json::Parser::parseChar(uint8_t which)
{
    const uint8_t* ch = m_stream.peekByte();
    if (ch == 0) {
        endOfFile();
    }
    if (*ch != which) {
        syntaxError();
    }
}

/** Internal - Expect two characters.
    Makes sure that we are about to read one of two characters (but does not consume it).
    If this assumption fails, throws an appropriate exception (syntaxError, endOfFile).
    \param first First character to expect
    \param second First character to expect
    \retval true Found the first character
    \retval false Found the second character */
bool
afl::io::json::Parser::parseChar(uint8_t first, uint8_t second)
{
    const uint8_t* ch = m_stream.peekByte();
    if (ch == 0) {
        endOfFile();
    } else if (*ch == first) {
        return true;
    } else if (*ch == second) {
        return false;
    } else {
        syntaxError();
    }

    // Not reached; silence warning.
    return false;
}

/** Internal - Parse an Unicode Escape.
    Reads the four hex digits that make up the Unicode escape.
    \return Unicode value */
uint32_t
afl::io::json::Parser::parseUnicodeEscape()
{
    uint32_t result = 0;
    for (int i = 0; i < 4; ++i) {
        // Read character
        const uint8_t* ch = m_stream.readByte();
        if (ch == 0) {
            endOfFile();
        }

        // Evaluate it
        result <<= 4;
        if (*ch >= '0' && *ch <= '9') {
            result += (*ch - '0');
        } else if (*ch >= 'A' && *ch <= 'F') {
            result += (*ch - 'A' + 10);
        } else if (*ch >= 'a' && *ch <= 'f') {
            result += (*ch - 'a' + 10);
        } else {
            syntaxError();
        }
    }

    return result;
}

/** Internal - Expect a keyword.
    Makes sure that we are about to read the specified keyword, and consumes it.
    If this assumption fails, throws an appropriate exception (syntaxError, endOfFile).

    Upon entry, read pointer points at the first character of the keyword.
    Upon exit, read pointer points after the last character of the keyword.
    \param kw Keyword to expect */
void
afl::io::json::Parser::parseKeyword(const char* kw)
{
    // Check the keyword
    while (*kw) {
        const uint8_t* ch = m_stream.readByte();
        if (ch == 0) {
            endOfFile();
        } else if (*ch != uint8_t(*kw)) {
            syntaxError();
        }
        ++kw;
    }

    // The keyword must not be followed by an identifier character!
    const uint8_t* ch = m_stream.peekByte();
    if (ch != 0
        && ((*ch >= 'a' && *ch <= 'z')
            || (*ch >= 'A' && *ch <= 'Z')
            || *ch == '_'
            || (*ch >= '0' && *ch <= '9')))
    {
        syntaxError();
    }
}
