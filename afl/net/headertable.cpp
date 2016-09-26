/**
  *  \file afl/net/headertable.cpp
  *  \brief Class afl::net::HeaderTable
  */

#include "afl/net/headertable.hpp"

afl::net::HeaderTable::HeaderTable()
    : m_headers()
{ }

afl::net::HeaderTable::~HeaderTable()
{ }

void
afl::net::HeaderTable::set(const String_t& name, const String_t& value)
{
    if (HeaderField* f = get(name)) {
        f->setValue(value);
    } else {
        m_headers.push_back(HeaderField(name, value));
    }
}

void
afl::net::HeaderTable::add(const String_t& name, const String_t& value)
{
    add(HeaderField(name, value));
}

void
afl::net::HeaderTable::add(const HeaderField& field)
{
    m_headers.push_back(field);
}

const afl::net::HeaderField*
afl::net::HeaderTable::get(const String_t& name) const
{
    return const_cast<HeaderTable*>(this)->get(name);
}

afl::net::HeaderField*
afl::net::HeaderTable::get(const String_t& name)
{
    for (size_t i = 0, n = m_headers.size(); i < n; ++i) {
        if (afl::string::strCaseCompare(name, m_headers[i].getName()) == 0) {
            return &m_headers[i];
        }
    }
    return 0;
}

void
afl::net::HeaderTable::enumerateHeaders(HeaderConsumer& consumer) const
{
    for (size_t i = 0, n = m_headers.size(); i < n; ++i) {
        consumer.handleHeader(m_headers[i].getName(), m_headers[i].getValue());
    }
}

void
afl::net::HeaderTable::writeHeaders(const String_t& dataName, afl::io::DataSink& sink) const
{
    static const uint8_t COLONSP[] = { ':', ' ' };
    static const uint8_t CRLF[] = { '\r', '\n' };
    afl::base::ConstBytes_t bytes;
    for (size_t i = 0, n = m_headers.size(); i < n; ++i) {
        bytes = afl::string::toBytes(m_headers[i].getName());
        sink.handleData(dataName, bytes);
        bytes = COLONSP;
        sink.handleData(dataName, bytes);
        bytes = afl::string::toBytes(m_headers[i].getValue());
        sink.handleData(dataName, bytes);
        bytes = CRLF;
        sink.handleData(dataName, bytes);
    }
}

void
afl::net::HeaderTable::handleHeader(String_t name, String_t value)
{
    add(name, value);
}
