/**
  *  \file afl/io/xml/writer.cpp
  *  \brief Class afl::io::xml::Writer
  */

#include "afl/io/xml/writer.hpp"
#include "afl/data/namemap.hpp"
#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"

using afl::data::NameMap;
using afl::io::DataSink;
using afl::string::toBytes;

afl::io::xml::Writer::Writer(DataSink& sink)
    : Visitor(),
      m_sink(sink)
{ }

afl::io::xml::Writer::~Writer()
{ }

void
afl::io::xml::Writer::visitPI(const PINode& node)
{
    m_sink.handleFullData(toBytes("<?"));
    m_sink.handleFullData(toBytes(node.getName()));
    writeAttributes(node);
    m_sink.handleFullData(toBytes("?>"));
}

void
afl::io::xml::Writer::visitTag(const TagNode& node)
{
    m_sink.handleFullData(toBytes("<"));
    m_sink.handleFullData(toBytes(node.getName()));
    writeAttributes(node);

    const Nodes_t& ch = node.getChildren();
    if (ch.empty()) {
        m_sink.handleFullData(toBytes("/>"));
    } else {
        m_sink.handleFullData(toBytes(">"));
        for (size_t i = 0, n = ch.size(); i < n; ++i) {
            visit(*ch[i]);
        }
        m_sink.handleFullData(toBytes("</"));
        m_sink.handleFullData(toBytes(node.getName()));
        m_sink.handleFullData(toBytes(">"));
    }
}

void
afl::io::xml::Writer::visitText(const TextNode& node)
{
    m_sink.handleFullData(toBytes(escape(node.get())));
}

String_t
afl::io::xml::Writer::escape(const String_t& in)
{
    String_t result;
    for (size_t i = 0; i < in.size(); ++i) {
        switch (in[i]) {
         case '<': result += "&lt;"; break;
         case '>': result += "&gt;"; break;
         case '&': result += "&amp;"; break;
         case '"': result += "&quot;"; break;
         default: result += in[i];
        }
    }
    return result;
}

template<typename NodeT>
inline void
afl::io::xml::Writer::writeAttributes(const NodeT& node)
{
    const NameMap& names = node.getAttributeNames();
    for (size_t i = 0, n = names.getNumNames(); i < n; ++i) {
        m_sink.handleFullData(toBytes(" "));
        m_sink.handleFullData(toBytes(names.getNameByIndex(i)));
        m_sink.handleFullData(toBytes("=\""));
        m_sink.handleFullData(toBytes(escape(node.getAttributeByIndex(i))));
        m_sink.handleFullData(toBytes("\""));
    }
}
