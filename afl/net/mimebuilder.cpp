/**
  *  \file afl/net/mimebuilder.cpp
  */

#include "afl/net/mimebuilder.hpp"
#include "afl/charset/base64.hpp"
#include "afl/charset/quotedprintable.hpp"

using afl::charset::QuotedPrintable;
using afl::charset::Base64;

namespace {
    const char CRLF[] = "\r\n";
}

afl::net::MimeBuilder::MimeBuilder(String_t ctype)
    : m_Content(),
      m_ContentType(ctype),
      m_Base64Accumulator(),
      m_PendingCRLF(false),
      m_State(StateHeader)
{
    if (!m_ContentType.empty()) {
        m_Content.push_back(Element(String_t(), ContentTypeElement));
    }
}

afl::net::MimeBuilder::~MimeBuilder()
{ }

void
afl::net::MimeBuilder::addHeader(String_t name, String_t value)
{
    // we should make sure we're actually in header state
    m_Content.push_back(Element(name + ": " + value + CRLF, TextElement));
}

void
afl::net::MimeBuilder::addLine(String_t line)
{
    flushHeaderAndStartBody();
    m_Content.push_back(Element(line + CRLF, TextElement));
}

void
afl::net::MimeBuilder::addBase64(afl::base::ConstBytes_t data)
{
    // We want 76 characters per line, that is 19 quads corresponding
    // to 19 input triplets i.e. 57 input characters
    flushHeaderAndStartBody();
    while (!data.empty()) {
        size_t room = 57 - m_Base64Accumulator.size();
        m_Base64Accumulator.append(afl::string::fromBytes(data.split(room)));
        if (m_Base64Accumulator.size() == 57) {
            flushBase64();
        }
    }
}

void
afl::net::MimeBuilder::addLineQP(String_t line)
{
    // FIXME: we should split long lines
    flushHeaderAndStartBody();

    String_t::size_type pos = 0, n;
    while ((n = line.find('\n', pos)) != line.npos) {
        m_Content.push_back(Element(afl::string::fromBytes(QuotedPrintable().encode(afl::string::toMemory(String_t(line, pos, n-pos)))) + CRLF, TextElement));
        pos = n+1;
    }
    m_Content.push_back(Element(afl::string::fromBytes(QuotedPrintable().encode(afl::string::toMemory(String_t(line, pos)))) + CRLF, TextElement));
}

void
afl::net::MimeBuilder::addRawData(afl::base::ConstBytes_t data)
{
    // When sending raw data, we must be extra careful when choosing the boundary line,
    // to not accidentally trigger a split in the middle of the file.
    // finish() checks the beginnings of all TextElement's.
    // We therefore split the binary data into Element's in such a way
    // that possible conflict points start a new Element.

    // RFC 2046 says a boundary is recognized only after CRLF.
    // For robustness, we assume conflict points at every LF.
    // This should be as robust network-wise as it can get.
    // It could be made a little more memory-efficient by splitting only at "\n--" places.
    flushHeaderAndStartBody();

    size_t n;
    while ((n = data.find('\n')) < data.size()) {
        m_Content.push_back(Element(afl::string::fromBytes(data.split(n+1)), TextElement));
    }
    if (!data.empty()) {
        m_Content.push_back(Element(afl::string::fromBytes(data), TextElement));
    }
    m_PendingCRLF = true;
}

void
afl::net::MimeBuilder::addBoundary()
{
    flushHeaderAndStartBody();
    flushBase64();
    m_Content.push_back(Element(String_t(), BoundaryElement));
    m_State = StateHeader;
}

void
afl::net::MimeBuilder::finish()
{
    // figure out a good boundary
    String_t boundary = "--000";
    while (1) {
        // is it already in use?
        bool ok = true;
        for (std::list<Element>::iterator i = m_Content.begin(); i != m_Content.end(); ++i) {
            // The additional size test is because Borland C++ doesn't like
            // compare(a,b,c) being called on a string shorter than a+b.
            if (i->m_Text.size() >= boundary.size() && i->m_Text.compare(0, boundary.size(), boundary) == 0) {
                ok = false;
                break;
            }
        }

        // nope, use it.
        if (ok) {
            break;
        }

        // yes. Guess a new one.
        String_t::size_type i = 2;
        while (i < boundary.size() && boundary[i] == '9') {
            boundary[i] = '0';
            ++i;
        }
        if (i < boundary.size())
            ++boundary[i];
        else
            boundary += '0';
    }

    // now fill in the content
    String_t* last_boundary = 0;
    for (std::list<Element>::iterator i = m_Content.begin(); i != m_Content.end(); ++i) {
        if (i->m_Type == ContentTypeElement) {
            i->m_Text = "Content-Type: " + m_ContentType + "; boundary=" + boundary.substr(2) + CRLF;
        } else if (i->m_Type == BoundaryElement) {
            i->m_Text = boundary + CRLF;
            last_boundary = &i->m_Text;
        }
    }
    if (last_boundary) {
        *last_boundary = boundary + "--" + CRLF;
    }
}

size_t
afl::net::MimeBuilder::getTotalSize() const
{
    // ex computeSize
    size_t sum = 0;
    for (std::list<Element>::const_iterator i = m_Content.begin(); i != m_Content.end(); ++i) {
        sum += i->m_Text.size();
    }
    return sum;
}

void
afl::net::MimeBuilder::write(afl::io::DataSink& out, bool escapeDots) const
{
    static const uint8_t dot[] = {'.'};
    for (std::list<Element>::const_iterator i = m_Content.begin(); i != m_Content.end(); ++i) {
        if (escapeDots && !i->m_Text.empty() && i->m_Text[0] == '.') {
            out.handleFullData(dot);
        }
        out.handleFullData(afl::string::toBytes(i->m_Text));
    }
}

String_t
afl::net::MimeBuilder::removeInitialHeaders()
{
    String_t result;
    while (!m_Content.empty() && m_Content.front().m_Type != BoundaryElement) {
        result += m_Content.front().m_Text;
        m_Content.pop_front();
    }
    return result;
}


/** Ensure we are in the body of the message. In case we're still in
    the header, adds the blank line that separates them. */
void
afl::net::MimeBuilder::flushHeaderAndStartBody()
{
    if (m_State == StateHeader) {
        m_State = StateBody;
        m_Content.push_back(Element(String_t(CRLF), TextElement));
    }
}

/** Flush base64 buffer. In case we have pending base64 characters,
    write them out to the message. */
void
afl::net::MimeBuilder::flushBase64()
{
    if (m_Base64Accumulator.size()) {
        m_Content.push_back(Element(afl::string::fromBytes(Base64().encode(afl::string::toMemory(m_Base64Accumulator))) + CRLF, TextElement));
        m_Base64Accumulator.clear();
    }
    if (m_PendingCRLF) {
        m_Content.push_back(Element(CRLF, TextElement));
        m_PendingCRLF = false;
    }
}
