/**
  *  \file afl/io/resp/parser.cpp
  *  \brief Class afl::io::resp::Parser
  *
  *  Normative reference:
  *  - http://redis.io/topics/protocol
  *
  *  RESP Simple Strings
  *    Simple Strings are encoded in the following way: a plus character,
  *    followed by a string that cannot contain a CR or LF character (no newlines are allowed),
  *    terminated by CRLF (that is "\r\n").
  *  RESP Errors
  *    RESP has a specific data type for errors.
  *    Actually errors are exactly like RESP Simple Strings,
  *    but the first character is a minus '-' character instead of a plus.
  *    The real difference between Simple Strings and Errors in RESP is that errors are treated
  *    by clients as exceptions, and the string that composes the Error type is the error message itself.
  *    The basic format is: "-Error message\r\n"
  *  RESP Integers
  *    This type is just a CRLF terminated string representing an integer, prefixed by a ":" byte.
  *    For example ":0\r\n", or ":1000\r\n" are integer replies.
  *  RESP Bulk Strings
  *    Bulk Strings are used in order to represent a single binary safe string up to 512 MB in length.
  *    Bulk Strings are encoded in the following way:
  *    - A "$" byte followed by the number of bytes composing the string (a prefixed length), terminated by CRLF.
  *    - The actual string data.
  *    - A final CRLF.
  *    So the string "foobar" is encoded as follows: "$6\r\nfoobar\r\n"
  *    RESP Bulk Strings can also be used in order to signal non-existence of a value using a special
  *    format that is used to represent a Null value.
  *    In this special format the length is -1, and there is no data, so a Null is represented as: "$-1\r\n"
  *  RESP Arrays
  *    Clients send commands to the Redis server using RESP Arrays.
  *    Similarly certain Redis commands returning collections of elements to the client use RESP Arrays are reply type.
  *    RESP Arrays are sent using the following format:
  *    - A * character as the first byte, followed by the number of elements in the array as a decimal number,
  *      followed by CRLF.
  *    - An additional RESP type for every element of the Array.
  *    So an empty Array is just the following: "*0\r\n"
  *    While an array of two RESP Bulk Strings "foo" and "bar" is encoded as: "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
  *    The concept of Null Array exists as well, and is an alternative way to specify a Null value: "*-1\r\n"
  */

#include <climits>
#include <cassert>
#include "afl/io/resp/parser.hpp"
#include "afl/data/segment.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/string/char.hpp"
#include "afl/string/messages.hpp"

// Size of stream-based parser:
// text    data     bss     dec     hex filename
//  2747       0       0    2747     abb afl/io/resp/parser1.o

// Size of push parser:
// text    data     bss     dec     hex filename
//  6516       0       0    6516    1974 afl/io/resp/parser1.o (original)
//  5732       0       0    5732    1664 afl/io/resp/parser1.o (PtrVector->auto_ptr)
//  6952       0       0    6952    1b28 afl/io/resp/parser1.o (add ShortState, d'ooh)

namespace {
    const char SOURCE_ID[] = "<Parser>";
}

/* Base class for a parser state. */
class afl::io::resp::Parser::State {
 public:
    virtual ~State()
        { }

    /* Consume data.
       This function must
       - consume data
       - optionally change to a new state (this destroys this state!)
       - optionally produce output and reset the state (this destroys this state!)
       It can exit by throwing an exception.
       \param parser Invoking instance
       \param data [in/out] Data to consume */
    virtual void handleData(Parser& parser, afl::base::ConstBytes_t& data) = 0;

 protected:
    void syntaxError()
        {
            throw afl::except::FileFormatException(SOURCE_ID, afl::string::Messages::syntaxError());
        }
};

/* Root state. Reads the type character and produces the next state. */
class afl::io::resp::Parser::RootState : public State {
 public:
    virtual void handleData(Parser& parser, afl::base::ConstBytes_t& data);
};

/* String state. Reads a single line and produces a String or Error result. */
class afl::io::resp::Parser::StringState : public State {
 public:
    StringState(bool success)
        : State(),
          m_success(success),
          m_value()
        { }
    virtual void handleData(Parser& parser, afl::base::ConstBytes_t& data);

 private:
    const bool m_success;
    String_t m_value;
};

/* Integer state. Reads a number and produces either an integer result, or a bulk/array state. */
class afl::io::resp::Parser::IntState : public State {
 public:
    enum Kind {
        Integer,
        Bulk,
        Array
    };
    IntState(Kind kind)
        : State(),
          m_kind(kind),
          m_state(Virgin),
          m_value(0)
        { }
    virtual void handleData(Parser& parser, afl::base::ConstBytes_t& data);

 private:
    const Kind m_kind;

    enum { Virgin, Positive, PositiveDigits, Negative, NegativeDigits } m_state;
    int32_t m_value;
};

/* Bulk state. Read bulk data followed by a newline and produces a string result. */
class afl::io::resp::Parser::BulkState : public State {
 public:
    BulkState(uint32_t size)
        : State(),
          m_size(size),
          m_value()
        { }
    virtual void handleData(Parser& parser, afl::base::ConstBytes_t& data)
        {
            afl::base::ConstBytes_t now(data.splitUpdate(m_size));
            if (!now.empty()) {
                m_value.append(reinterpret_cast<const char*>(now.unsafeData()), now.size());
            }
            if (m_size == 0) {
                while (const uint8_t* pc = data.eat()) {
                    if (*pc == '\r') {
                        // ignore
                    } else if (*pc == '\n') {
                        parser.m_pValue.reset(parser.m_factory.createString(m_value));
                        parser.m_pState.reset();
                        break;
                    } else {
                        syntaxError();
                    }
                }
            }
        }

 private:
    uint32_t m_size;
    String_t m_value;
};

/* Array state. Reads array content and produces a vector result.
   Note that this class must not be instantiated for 0-byte arrays
   because it would otherwise not read at least one byte. */
class afl::io::resp::Parser::ArrayState : public State {
 public:
    ArrayState(afl::data::ValueFactory& factory, uint32_t size)
        : State(),
          m_size(size),
          m_value(),
          m_parser(factory)
        { }
    virtual void handleData(Parser& parser, afl::base::ConstBytes_t& data)
        {
            // Consume data until we have filled the array
            while (!data.empty() && m_size > 0) {
                if (m_parser.handleData(data)) {
                    m_value.pushBackNew(m_parser.extract());
                    --m_size;
                }
            }

            // Produce output
            if (m_size == 0) {
                parser.m_pValue.reset(parser.m_factory.createVector(m_value));
                parser.m_pState.reset();
            }
        }

 private:
    uint32_t m_size;
    afl::data::Segment m_value;
    Parser m_parser;
};

/* Short state. Reads a line in short form and produces a vector result. */
class afl::io::resp::Parser::ShortState : public State {
 public:
    ShortState(const char ch)
        : State(),
          m_accumulator(1, ch),
          m_value()
        { }

    virtual void handleData(Parser& parser, afl::base::ConstBytes_t& data)
        {
            while (const uint8_t* pc = data.eat()) {
                const uint8_t c = *pc;
                if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                    // whitespace
                    if (m_accumulator.size() != 0) {
                        m_value.pushBackNew(parser.m_factory.createString(m_accumulator));
                        m_accumulator.clear();
                    }
                    if (c == '\n') {
                        parser.m_pValue.reset(parser.m_factory.createVector(m_value));
                        parser.m_pState.reset();
                        break;
                    }
                } else {
                    // character
                    m_accumulator.append(1, c);
                }
            }
        }

 private:
    String_t m_accumulator;
    afl::data::Segment m_value;
};


void
afl::io::resp::Parser::RootState::handleData(Parser& parser, afl::base::ConstBytes_t& data)
{
    if (const uint8_t* pc = data.eat()) {
        switch (*pc) {
         case '+':
            parser.m_pState.reset(new StringState(true));
            break;

         case '-':
            parser.m_pState.reset(new StringState(false));
            break;

         case ':':
            parser.m_pState.reset(new IntState(IntState::Integer));
            break;

         case '$':
            parser.m_pState.reset(new IntState(IntState::Bulk));
            break;

         case '*':
            parser.m_pState.reset(new IntState(IntState::Array));
            break;

         default:
            if (parser.m_acceptShort && (afl::string::charIsUpper(*pc) || afl::string::charIsLower(*pc))) {
                parser.m_pState.reset(new ShortState(*pc));
            } else {
                syntaxError();
            }
            break;
        }
    }
}

void
afl::io::resp::Parser::StringState::handleData(Parser& parser, afl::base::ConstBytes_t& data)
{
    while (const uint8_t* pc = data.eat()) {
        if (*pc == '\r') {
            // ignore
        } else if (*pc == '\n') {
            // finish
            parser.m_pValue.reset(m_success ? parser.m_factory.createString(m_value) : parser.m_factory.createError(SOURCE_ID, m_value));
            parser.m_pState.reset();
            break;
        } else {
            m_value.append(1, char(*pc));
        }
    }
}

void
afl::io::resp::Parser::IntState::handleData(Parser& parser, afl::base::ConstBytes_t& data)
{
    while (const uint8_t* pc = data.eat()) {
        const uint8_t ch = *pc;
        if (ch == '+') {
            if (m_state != Virgin) {
                syntaxError();
            }
            m_state = Positive;
        } else if (ch == '-') {
            if (m_state != Virgin) {
                syntaxError();
            }
            m_state = Negative;
        } else if (ch >= '0' && ch <= '9') {
            const int32_t digit = (ch - '0');
            if (m_state == Negative || m_state == NegativeDigits) {
                if (m_value < INT_MIN/10 || (m_value != 0 && 10*m_value - INT_MIN < digit)) {
                    syntaxError();
                }
                m_value = 10*m_value - digit;
                m_state = NegativeDigits;
            } else {
                if (m_value > INT_MAX/10 || INT_MAX - 10*m_value < digit) {
                    syntaxError();
                }
                m_value = 10*m_value + digit;
                m_state = PositiveDigits;
            }
        } else if (ch == '\r') {
            // ignore
        } else if (ch == '\n') {
            // end
            if (m_state != PositiveDigits && m_state != NegativeDigits) {
                syntaxError();
            }

            // Save member variables locally because we're going to die
            switch (m_kind) {
             case Integer:
                parser.m_pValue.reset(parser.m_factory.createInteger(m_value));
                parser.m_pState.reset();
                break;

             case Array:
                if (m_value < 0) {
                    // Null array
                    parser.m_pValue.reset();
                    parser.m_pState.reset();
                } else if (m_value == 0) {
                    // FIXME: can we avoid this special case?
                    // We must not create a state that consumes 0 bytes because Parser::handleData may exit in that case.
                    // A 0-length array does consume 0 bytes.
                    afl::data::Segment seg;
                    parser.m_pValue.reset(parser.m_factory.createVector(seg));
                    parser.m_pState.reset();
                } else {
                    parser.m_pState.reset(new ArrayState(parser.m_factory, uint32_t(m_value)));
                }
                break;

             case Bulk:
                if (m_value < 0) {
                    // Null string
                    parser.m_pValue.reset();
                    parser.m_pState.reset();
                } else {
                    parser.m_pState.reset(new BulkState(uint32_t(m_value)));
                }
                break;
            }
            break;
        } else {
            syntaxError();
        }
    }
}

/********************************* Parser ********************************/

afl::io::resp::Parser::Parser(afl::data::ValueFactory& factory)
    : DataSink(),
      m_acceptShort(false),
      m_factory(factory),
      m_pState(new RootState()),
      m_pValue()
{ }

afl::io::resp::Parser::~Parser()
{ }

bool
afl::io::resp::Parser::handleData(afl::base::ConstBytes_t& data)
{
    // Process data
    while (m_pState.get() != 0 && !data.empty()) {
        m_pState->handleData(*this, data);
    }

    // We're done when we no longer have a state
    return m_pState.get() == 0;
}

void
afl::io::resp::Parser::setAcceptShortForm(bool acceptShort)
{
    m_acceptShort = acceptShort;
}

afl::data::Value*
afl::io::resp::Parser::extract()
{
    assert(m_pState.get() == 0);
    m_pState.reset(new RootState());
    return m_pValue.release();
}
