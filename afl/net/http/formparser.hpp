/**
  *  \file afl/net/http/formparser.hpp
  */
#ifndef AFL_AFL_NET_HTTP_FORMPARSER_HPP
#define AFL_AFL_NET_HTTP_FORMPARSER_HPP

#include "afl/io/datasink.hpp"
#include "afl/net/headerconsumer.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace http {

    /** Parser for application/x-www-form-urlencoded.
        When fed some data, disassembles that and hands it to a HeaderConsumer.
        After having given all data, call handleDataComplete() to finish the process. */
    class FormParser : public afl::io::DataSink {
     public:
        /** Constructor.
            \param consumer Consumer to receive the parameters. */
        FormParser(HeaderConsumer& consumer);

        /** Destructor. */
        ~FormParser();

        /** Finish.
            Call this after having called handleData() for the last time.
            This will finish the process, and reset the FormParser back to its initial state. */
        void handleDataComplete();

        // DataSink:
        virtual bool handleData(afl::base::ConstBytes_t& data);
        
     private:
        HeaderConsumer& m_consumer;

        enum State {
            Key,
            Value
        };
        State m_state;

        String_t m_key;
        String_t m_value;
    };

} } }

#endif
