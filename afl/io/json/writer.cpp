/**
  *  \file afl/io/json/writer.cpp
  *  \brief Class afl::io::json::Writer
  */

#include "afl/io/json/writer.hpp"
#include "afl/string/format.hpp"
#include "afl/data/namemap.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/vector.hpp"

afl::io::json::Writer::Writer(afl::io::DataSink& sink)
    : afl::data::Visitor(),
      m_lineLength(0),
      m_indentStep(0),
      m_sink(sink),
      m_currentLineLength(0),
      m_currentIndentLevel(0),
      m_currentAllowNewline(false)
{ }

afl::io::json::Writer::~Writer()
{ }

void
afl::io::json::Writer::setLineLength(size_t lineLength)
{
    m_lineLength = lineLength;
}

void
afl::io::json::Writer::setIndentStep(size_t indentStep)
{
    m_indentStep = indentStep;
}


void
afl::io::json::Writer::visitString(const String_t& str)
{
    String_t fmt = "\"";
    for (size_t i = 0; i < str.size(); ++i) {
        uint8_t ch = static_cast<uint8_t>(str[i]);
        if (ch == '\\') {
            fmt += "\\\\";
        } else if (ch == '"') {
            fmt += "\\\"";
        } else if (ch >= 32) {
            fmt += char(ch);
        } else if (ch == '\n') {
            fmt += "\\n";
        } else if (ch == '\r') {
            fmt += "\\r";
        } else if (ch == '\b') {
            fmt += "\\b";
        } else if (ch == '\t') {
            fmt += "\\t";
        } else if (ch == '\f') {
            fmt += "\\f";
        } else {
            // Applies to other control characters such as \0, \033, etc.
            fmt += afl::string::Format("\\u%04X", ch);
        }
    }
    fmt += "\"";
    write(fmt, 0);
}

void
afl::io::json::Writer::visitInteger(int32_t iv)
{
    write(afl::string::Format("%d", iv), 0);
}

void
afl::io::json::Writer::visitFloat(double fv)
{
    write(afl::string::Format("%.25g", fv), 0);
}

void
afl::io::json::Writer::visitBoolean(bool bv)
{
    write(bv ? "true" : "false", 0);
}

void
afl::io::json::Writer::visitHash(const afl::data::Hash& hv)
{
    write("{", NewlineAfter | IndentAfter);
    const afl::data::NameMap& keys   = hv.getKeys();
    const afl::data::Segment& values = hv.getValues();
    for (afl::data::NameMap::Index_t i = 0, n = keys.getNumNames(); i < n; ++i) {
        if (i != 0) {
            write(",", NewlineAfter);
        }
        visitString(keys.getNameByIndex(i));
        write(":", 0);
        visit(values[i]);
    }
    write("}", NewlineBefore | UndentBefore);
}

void
afl::io::json::Writer::visitVector(const afl::data::Vector& vv)
{
    write("[", NewlineAfter | IndentAfter);
    for (size_t i = 0, n = vv.size(); i < n; ++i) {
        if (i != 0) {
            write(",", NewlineAfter);
        }
        visit(vv[i]);
    }
    write("]", NewlineBefore | UndentBefore);
}

void
afl::io::json::Writer::visitOther(const afl::data::Value& /*other*/)
{
    write("null", 0);
}

void
afl::io::json::Writer::visitError(const String_t& /*source*/, const String_t& /*str*/)
{
    write("null", 0);
}

void
afl::io::json::Writer::visitNull()
{
    write("null", 0);
}

void
afl::io::json::Writer::write(const String_t& str, uint32_t flags)
{
    static const uint8_t NEWLINE[] = { '\n' };

    // Process un-indentation
    if ((flags & UndentBefore) != 0) {
        m_currentIndentLevel -= m_indentStep;
    }

    // Newline?
    // A newline is allowed if this token allows one before, or the previous one allows one after.
    // A newline is written if we're indenting (pretty-printing), or will exceed the line length.
    if (((flags & NewlineBefore) != 0
         || m_currentAllowNewline)
        && (m_indentStep > 0
            || (m_lineLength != 0 && m_currentLineLength + str.size() > m_lineLength)))
    {
        afl::base::ConstBytes_t newlineBytes(NEWLINE);
        m_sink.handleData(newlineBytes);

        // Indentation
        if (m_currentIndentLevel > 0) {
            // Prepare some bytes
            uint8_t buffer[128];
            afl::base::Bytes_t bytes(buffer);
            bytes.fill(' ');

            // Write them
            size_t n = m_currentIndentLevel;
            while (n > 0) {
                afl::base::ConstBytes_t indentBytes(bytes.subrange(0, n));
                n -= indentBytes.size();
                m_sink.handleData(indentBytes);
            }
        }

        m_currentLineLength = m_currentIndentLevel;
    }

    // Actual element
    afl::base::ConstBytes_t stringBytes = afl::string::toBytes(str);
    m_sink.handleData(stringBytes);
    m_currentLineLength += str.size();

    // Post-process
    if ((flags & IndentAfter) != 0) {
        m_currentIndentLevel += m_indentStep;
    }
    m_currentAllowNewline = ((flags & NewlineAfter) != 0);
}
