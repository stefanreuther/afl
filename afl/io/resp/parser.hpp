/**
  *  \file afl/io/resp/parser.hpp
  *  \brief Class afl::io::resp::Parser
  */
#ifndef AFL_AFL_IO_RESP_PARSER_HPP
#define AFL_AFL_IO_RESP_PARSER_HPP

#include <memory>
#include "afl/data/valuefactory.hpp"
#include "afl/io/datasink.hpp"
#include "afl/container/ptrvector.hpp"

namespace afl { namespace io { namespace resp {

    /** RESP Parser.
        This is a push-style parser that decodes RESP (Redis Serialisation Protocol).
        All data types are supported:
        - simple strings ("+") are handled as strings
        - errors ("-") are reported as error values
        - integers (":") are handled as integers
        - null bulk strings/arrays are handled as null values
        - bulk strings ("$") are handled as strings
        - arrays ("*") are handled as vectors */
    class Parser : public DataSink {
     public:
        /** Constructor.
            \param factory ValueFactory to create actual objects */
        explicit Parser(afl::data::ValueFactory& factory);

        /** Destructor. */
        ~Parser();

        /** Consume data.
            \param data [in/out] On input, data to process. On output, remaining unprocessed data.
            \retval false More data required (%data is empty)
            \retval true No more data required (%data has unprocessed data), call extract() to obtain result */
        virtual bool handleData(afl::base::ConstBytes_t& data);

        /** Set acceptance of short form.
            In short form, we accept unstructured lines which are broken into words as array of string.
            \param acceptShort flag */
        void setAcceptShortForm(bool acceptShort);

        /** Extract result.
            Must only be called after handleData() returned true.
            You receive ownership of the object.
            This method resets the parser's state, so you can call handleData() again to parse more data.
            \return parsed object (can be null) */
        afl::data::Value* extract();

     private:
        bool m_acceptShort;
        afl::data::ValueFactory& m_factory;

        class State;
        friend class State;
        class RootState;
        friend class RootState;
        class StringState;
        friend class StringState;
        class IntState;
        friend class IntState;
        class BulkState;
        friend class BulkState;
        class ArrayState;
        friend class ArrayState;
        class ShortState;
        friend class ShortState;

        std::auto_ptr<State> m_pState;
        std::auto_ptr<afl::data::Value> m_pValue;
    };

} } }

#endif
