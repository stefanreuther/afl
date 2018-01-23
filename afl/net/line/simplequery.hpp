/**
  *  \file afl/net/line/simplequery.hpp
  *  \brief Class afl::net::line::SimpleQuery
  */
#ifndef AFL_AFL_NET_LINE_SIMPLEQUERY_HPP
#define AFL_AFL_NET_LINE_SIMPLEQUERY_HPP

#include "afl/net/line/linehandler.hpp"

namespace afl { namespace net { namespace line {

    /** LineHandler as client: Simple query.
        Implements a protocol where an interaction consists of a single line sent to the other side,
        receiving data until the connection ends.
        An example of such a protocol would be HTTP/0.9.

        This LineHandler is intended to be used with Client. */
    class SimpleQuery : public LineHandler {
     public:
        /** Constructor.
            \param query Query line to send */
        explicit SimpleQuery(const String_t& query);

        /** Destructor. */
        virtual ~SimpleQuery();

        // LineHandler:
        virtual bool handleOpening(LineSink& response);
        virtual bool handleLine(const String_t& line, LineSink& response);
        virtual void handleConnectionClose();

        /** Get result.
            Call this after using this SimpleQuery with Client::call().
            \return all received lines, separated and terminated by '\n' (independant from line separator used on the network) */
        const String_t& getResult() const;

     private:
        String_t m_query;
        String_t m_result;
    };

} } }

#endif
