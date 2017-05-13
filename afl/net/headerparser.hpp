/**
  *  \file afl/net/headerparser.hpp
  *  \brief Class afl::net::HeaderParser
  */
#ifndef AFL_AFL_NET_HEADERPARSER_HPP
#define AFL_AFL_NET_HEADERPARSER_HPP

#include "afl/string/string.hpp"
#include "afl/io/datasink.hpp"

namespace afl { namespace net {

    class HeaderConsumer;

    /** MIME header parser.
        This is a push-parser for MIME-style headers.
        It parses headers from a sequence of data blocks being pushed into the handleData() function,
        and converts these into callbacks on a HeaderConsumer object.

        - invalid lines are ignored
        - header folding is implemented
        - comments, quotes and encoded-words are not specially handled, this is up to the user
        - no special provisions have been made for duplicate headers;
          multiple headers with the same keyword generate repeated callbacks
          and it is up to the callback to merge or otherwise process them. */
    class HeaderParser : public afl::io::DataSink {
     public:
        /** Constructor.
            \param consumer Consumer */
        HeaderParser(HeaderConsumer& consumer);

        /** Destructor. */
        ~HeaderParser();

        /** Process data.
            Parses the data and calls the consumer's handleHeader() method for each recognized header.
            \param bytes [in/out] data to process. On output: unprocessed data.
            \retval true header ends, unprocessed bytes have been left in %bytes
            \retval false header incomplete, data has been processed completely, feed more data */
        bool handleData(afl::base::ConstBytes_t& bytes);

        /** Check for errors.
            \retval true there were syntax errors processing the header
            \retval false header parsed successfully */
        bool hasErrors() const;

        /** Check for folding.
            \retval true there were folded lines in the header (deprecated in HTTP, common in mail/news)
            \retval false no header folding detected */
        bool hasFolding() const;

        /** Check for completion.
            This function mirrors the return value of handleData().
            \retval true complete header has been processed
            \retval false need more data */
        bool isCompleted() const;

     private:
        HeaderConsumer& m_consumer;
        bool m_hasErrors;
        bool m_hasFolding;
        String_t m_key;
        String_t m_value;

        enum State {
            /** Initial state. Looking for first key.
                - NL: FinalState
                - SP, colon: ErrorState
                - other: add to key, KeyState */
            InitialState,

            /** Key state. Reading a key.
                - NL: mark error, InitialState
                - SP: mark error, ErrorState
                - colon: ValueState
                - other: add to key */
            KeyState,

            /** Value state. Reading a value.
                - NL: NewlineState
                - SP, colon, other: add to value */
            ValueState,

            /** Newline state. Just read a newline; determine whether we proceed with the header or a new one.
                - NL: flush the key/value, FinalState
                - SP: add space to value, ValueState
                - colon: flush the key/value, ErrorState
                - other: flush key/value, initialize new key/value, KeyState */
            NewlineState,

            /** Error state. Found a bad line, just look for the newline.
                - NL: InitialState
                - SP, colon, other: ignore */
            ErrorState,

            /** Final state. Don't read anymore. */
            FinalState
        };
        State m_state;

        void flush();
    };

} }

inline bool
afl::net::HeaderParser::hasErrors() const
{
    return m_hasErrors;
}

inline bool
afl::net::HeaderParser::hasFolding() const
{
    return m_hasFolding;
}

inline bool
afl::net::HeaderParser::isCompleted() const
{
    return m_state == FinalState;
}

#endif
