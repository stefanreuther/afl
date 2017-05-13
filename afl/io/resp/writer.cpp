/**
  *  \file afl/io/resp/writer.cpp
  *  \brief Class afl::io::resp::Writer
  */

#include "afl/io/resp/writer.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/namemap.hpp"
#include "afl/data/segment.hpp"
#include "afl/string/format.hpp"

using afl::string::toMemory;

afl::io::resp::Writer::Writer(afl::io::DataSink& sink)
    : m_sink(sink)
{ }

afl::io::resp::Writer::~Writer()
{ }

void
afl::io::resp::Writer::sendError(const String_t& str)
{
    visitError("<resp::Writer>", str);
}

void
afl::io::resp::Writer::sendSuccess(const String_t& str)
{
    write(toMemory("+"));
    write(toMemory(str));
    write(toMemory("\r\n"));
}

void
afl::io::resp::Writer::visitString(const String_t& str)
{
    write(toMemory(afl::string::Format("$%d\r\n", str.size())));
    write(toMemory(str));
    write(toMemory("\r\n"));
}

void
afl::io::resp::Writer::visitInteger(int32_t iv)
{
    visitString(afl::string::Format("%d", iv));
}

void
afl::io::resp::Writer::visitFloat(double fv)
{
    visitString(afl::string::Format("%.25g", fv));
}

void
afl::io::resp::Writer::visitBoolean(bool bv)
{
    visitString(bv ? "1" : "0");
}

void
afl::io::resp::Writer::visitHash(const afl::data::Hash& hv)
{
    const afl::data::NameMap& keys   = hv.getKeys();
    const afl::data::Segment& values = hv.getValues();
    size_t n = keys.getNumNames();

    // Serialize hash as array with 2n elements
    write(toMemory(afl::string::Format("*%d\r\n", 2*n)));
    for (afl::data::NameMap::Index_t i = 0; i < n; ++i) {
        visitString(keys.getNameByIndex(i));
        visit(values[i]);
    }
}

void
afl::io::resp::Writer::visitVector(const afl::data::Vector& vv)
{
    visitSegment(vv);
}

void
afl::io::resp::Writer::visitOther(const afl::data::Value& /*other*/)
{
    // Unserializable. Serialize as null array so other side can distinguish it from actual null value.
    write(toMemory("*-1\r\n"));
}

void
afl::io::resp::Writer::visitError(const String_t& /*source*/, const String_t& str)
{
    // We can only write one line of error message, so if str contains more than one, trim it.
    String_t::size_type n = str.find_first_of("\r\n");
    if (n == String_t::npos) {
        n = str.size();
    }

    // Write
    write(toMemory("-"));
    write(toMemory(str).trim(n));
    write(toMemory("\r\n"));
}

void
afl::io::resp::Writer::visitNull()
{
    write(toMemory("$-1\r\n"));
}

void
afl::io::resp::Writer::visitSegment(const afl::data::Segment& seg)
{
    size_t n = seg.size();
    write(toMemory(afl::string::Format("*%d\r\n", n)));
    for (size_t i = 0; i < n; ++i) {
        visit(seg[i]);
    }
}

void
afl::io::resp::Writer::write(afl::string::ConstStringMemory_t str)
{
    afl::base::ConstBytes_t bytes = str.toBytes();
    m_sink.handleData(bytes); // FIXME: handleFullData?
}
