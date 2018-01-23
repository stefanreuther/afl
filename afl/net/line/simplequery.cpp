/**
  *  \file afl/net/line/simplequery.cpp
  *  \brief Class afl::net::line::SimpleQuery
  */

#include "afl/net/line/simplequery.hpp"
#include "afl/net/line/linesink.hpp"

// Constructor.
afl::net::line::SimpleQuery::SimpleQuery(const String_t& query)
    : m_query(query),
      m_result()
{ }

// Destructor.
afl::net::line::SimpleQuery::~SimpleQuery()
{ }

// Handle opening of a conversation.
bool
afl::net::line::SimpleQuery::handleOpening(LineSink& response)
{
    response.handleLine(m_query);
    return false;
}

// Handle a received line.
bool
afl::net::line::SimpleQuery::handleLine(const String_t& line, LineSink& /*response*/)
{
    m_result += line;
    m_result += '\n';
    return false;
}

// Handle closing of a connection.
void
afl::net::line::SimpleQuery::handleConnectionClose()
{ }

// Get result.
const String_t&
afl::net::line::SimpleQuery::getResult() const
{
    return m_result;
}
