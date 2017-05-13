/**
  *  \file afl/io/textsink.cpp
  *  \brief Class afl::io::TextSink
  */

#include "afl/io/textsink.hpp"
#include "afl/io/datasink.hpp"

afl::io::TextSink::TextSink(DataSink& sink)
    : m_sink(sink)
{ }

afl::io::TextSink&
afl::io::TextSink::operator<<(const char* str)
{
    m_sink.handleFullData(afl::string::toBytes(str));
    return *this;
}

afl::io::TextSink&
afl::io::TextSink::operator<<(const String_t& str)
{
    m_sink.handleFullData(afl::string::toBytes(str));
    return *this;
}

afl::io::TextSink&
afl::io::TextSink::operator<<(afl::string::ConstStringMemory_t mem)
{
    m_sink.handleFullData(mem.toBytes());
    return *this;
}
