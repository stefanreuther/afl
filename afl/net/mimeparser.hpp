/**
  *  \file afl/net/mimeparser.hpp
  *  \brief Class afl::net::MimeParser
  */
#ifndef AFL_AFL_NET_MIMEPARSER_HPP
#define AFL_AFL_NET_MIMEPARSER_HPP

#include <map>
#include <vector>
#include <memory>
#include "afl/base/memory.hpp"
#include "afl/base/optional.hpp"
#include "afl/base/types.hpp"
#include "afl/io/datasink.hpp"
#include "afl/string/string.hpp"
#include "afl/sys/loglistener.hpp"
#include "afl/net/headerconsumer.hpp"
#include "afl/net/headertable.hpp"
#include "afl/base/enumerator.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace net {

    /** MIME Message Parser.
        This splits a message into header and body, unfolds and decodes header fields, and decodes the body.

        Header fields are decoded into UTF-8.
        They are placed in a HeaderTable.
        MIME "encoded-words" are decoded for all header fields.

        As a special case, address literals from "Received" headers collected as a trace (getTrace()).

        The body is decoded from its transfer encoding.
        It is stored as a series of lines.
        There is an implicit newline BETWEEN body lines.
        That is, the empty body is represented by a one-element array with an empty string;
        a two-element array represents the body 'a[0] + "\n" + a[1]'.

        Carriage-return characters are ignored (unless explicitly produced by the transport encoding).

        Usage:
        - call addData() (any number of times)
        - call finish()
        - examine using the get() methods */
    class MimeParser : public afl::io::DataSink, private HeaderConsumer {
     public:
        /** Shortcut: vector of strings for message body. */
        typedef std::vector<String_t> BodyVec_t;


        /** Constructor.
            Makes an empty MimeParser. */
        MimeParser();

        /** Destructor. */
        ~MimeParser();

        /** Set logger.
            This logger will receive warning messages when there is a problem parsing the message.
            If no logger is set, these problems are silently ignored.
            \param pLog Logger */
        void setLog(afl::sys::LogListener* pLog);


        /*
         *  Manipulation/Setup
         */

        /** Process data.
            Parses the data into this MimeParser.
            \param bytes [in/out] data to process. On output: empty.
            \return always false (feed more data if you have); MimeParser does not leave unprocessed data */
        virtual bool handleData(afl::base::ConstBytes_t& bytes);

        /** Finish.
            Performs any possible postprocessing.
            Call this after a sequence of addData(), before examining the result.
            addData() may not be called after finish() has been called. */
        void finish();

        /** Clear.
            Reset this object into the state it had after construction. */
        void clear();


        /*
         *  Access
         */

        /** Access headers.
            \return headers. Keys are lower-case header names. */
        const HeaderTable& getHeaders() const;

        /** Access message body.
            \return message body */
        const BodyVec_t& getBody() const;

        /** Get header by name.
            \param name Header field name
            \return header field value (Nothing if not specified) */
        afl::base::Optional<String_t> getHeader(String_t name) const;

        /** Get file name.
            Looks for headers that indicate a file name of this MIME entity.
            \return file name (Nothing if none found) */
        afl::base::Optional<String_t> getFileName() const;

        /** Get entire body as string.
            \return body */
        String_t getBodyAsString() const;

        /** Get trace.
            The trace contains the IP addresses of all servers the message passed through,
            separated by "/". It is generated from "Received" headers.
            \return trace */
        String_t getTrace() const;

        /** Get parts of a multipart message.
            - if this is NOT a multipart message, returns null
            - if this is a multipart message, returns an enumerator.
              To obtain the parts, create a MimeParser object,
              and call getNextElement() on the iterator to populate the MimeParser with the individual parts in turn.
              Do NOT pass *this as the parameter.
            \return Iterator. Null if this is not a multipart message. */
        afl::base::Ptr<afl::base::Enumerator<MimeParser> > getParts() const;

     private:
        /** Headers of mail. Indexed by lowercase header field name. */
        HeaderTable m_headers;

        /** Trace. */
        String_t m_trace;

        /** Body of mail, one entry per line. */
        BodyVec_t m_body;

        /** Accumulator for parsing. */
        std::vector<uint8_t> m_accum;

        /** Parser state. */
        enum {
            StateHeader,
            StatePlainBody,
            StateBase64Body,
            StateQPBody
        } m_state;

        /** Logger. */
        afl::sys::LogListener* m_pLog;

        /** Header parser. */
        std::auto_ptr<DataSink> m_headerParser;

        // HeaderConsumer:
        virtual void handleHeader(String_t key, String_t value);

        void addDataHeader(afl::base::ConstBytes_t& data);
        void addDataPlainBody(afl::base::ConstBytes_t& data);
        void addDataBase64Body(afl::base::ConstBytes_t& data);
        void addDataQPBody(afl::base::ConstBytes_t& data);
        void addBody(const String_t& str);
        void flushBase64Body();

        void processReceivedHeader(const String_t& value);
    };

} }

#endif
