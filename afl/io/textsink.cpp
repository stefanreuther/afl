/**
  *  \file afl/io/textsink.cpp
  *  \brief Class afl::io::TextSink
  */

#include "afl/io/textsink.hpp"
#include "afl/io/datasink.hpp"

afl::io::TextSink::TextSink(String_t name, DataSink& sink)
    : m_name(name),
      m_sink(sink)
{ }

afl::io::TextSink&
afl::io::TextSink::operator<<(const char* str)
{
    afl::base::ConstBytes_t bytes(afl::string::toBytes(str));
    m_sink.handleData(m_name, bytes);
    return *this;
}

afl::io::TextSink&
afl::io::TextSink::operator<<(const String_t& str)
{
    afl::base::ConstBytes_t bytes(afl::string::toBytes(str));
    m_sink.handleData(m_name, bytes);
    return *this;
}

afl::io::TextSink&
afl::io::TextSink::operator<<(afl::string::ConstStringMemory_t mem)
{
    afl::base::ConstBytes_t bytes(mem.toBytes());
    m_sink.handleData(m_name, bytes);
    return *this;
}
