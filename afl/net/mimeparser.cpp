/**
  *  \file afl/net/mimeparser.cpp
  *  \brief Class afl::net::MimeParser
  */

#include <memory>
#include "afl/net/mimeparser.hpp"
#include "afl/charset/base64.hpp"
#include "afl/charset/charset.hpp"
#include "afl/charset/charsetfactory.hpp"
#include "afl/charset/defaultcharsetfactory.hpp"
#include "afl/charset/quotedprintable.hpp"
#include "afl/string/format.hpp"
#include "afl/net/headerparser.hpp"
#include "afl/string/char.hpp"

using afl::string::strLCase;
using afl::string::strTrim;
using afl::string::strLTrim;
using afl::string::Format;
using afl::net::MimeParser;

namespace {
    const char LOG_NAME[] = "net.mime";

    inline String_t decodeBase64(const String_t& in)
    {
        return afl::charset::Base64().decode(afl::string::toBytes(in));
    }

    inline String_t decodeBase64(afl::base::ConstBytes_t bytes)
    {
        return afl::charset::Base64().decode(bytes);
    }

    inline String_t decodeQP(const String_t& in)
    {
        return afl::charset::QuotedPrintable().decode(afl::string::toBytes(in));
    }

    inline String_t decodeQP(afl::base::ConstBytes_t bytes)
    {
        return afl::charset::QuotedPrintable().decode(bytes);
    }

    inline bool isEntirelyWhitespace(const String_t& str, size_t start, size_t end)
    {
        while (start < end) {
            if (!afl::string::charIsSpace(str[start])) {
                return false;
            }
            ++start;
        }
        return true;
    }

    /** Decode encoded-words from a header.
        \param header Incoming header line
        \return Decoded header, in UTF-8. */
    String_t decodeHeader(const String_t header, afl::sys::LogListener* pLog)
    {
        String_t result;
        String_t::size_type pos = 0, n;
        while ((n = header.find("=?", pos)) != String_t::npos) {
            // Append unencoded part UNLESS it's entirely whitespace after a previous encoded-word
            if (pos == 0 || !isEntirelyWhitespace(header, pos, n)) {
                result.append(header, pos, n-pos);
            }
            pos = n+2;

            // Found an encoded word. Format is:
            //   =?charset?encoding?text?=
            // Parse the charset.
            n = header.find('?', pos);
            if (n == String_t::npos) {
                // encoding error
                break;
            }
            String_t charset(strLCase(String_t(header, pos, n-pos)));
            pos = n+1;

            // Parse the encoding.
            n = header.find('?', pos);
            if (n == String_t::npos) {
                // encoding error
                break;
            }
            String_t encoding(strLCase(String_t(header, pos, n-pos)));
            pos = n+1;

            // Locate the terminator
            n = header.find("?=", pos);
            if (n == String_t::npos) {
                // encoding error
                break;
            }
            String_t word(header, pos, n-pos);
            pos = n+2;

            // Decode encoding
            if (encoding == "b") {
                // base-64
                word = decodeBase64(word);
            } else {
                // treat as quoted-printable
                for (String_t::size_type i = 0; i < word.size(); ++i) {
                    if (word[i] == '_') {
                        word[i] = ' ';
                    }
                }
                word = decodeQP(word);
            }

            // Decode charset
            // FIXME: hardcoded DefaultCharsetFactory
            std::auto_ptr<afl::charset::Charset> p(afl::charset::DefaultCharsetFactory().createCharset(charset));
            if (p.get() != 0) {
                word = p->decode(afl::string::toBytes(word));
            } else {
                if (pLog != 0) {
                    pLog->write(afl::sys::LogListener::Warn, LOG_NAME, Format("unknown character set '%s' (ignored)", charset));
                }
            }
            result.append(word);
        }
        result.append(header.substr(pos));
        return result;
    }

    /*
     *  Enumerator implementation that produces parts of a multipart message
     */
    class PartIterator : public afl::base::Enumerator<MimeParser> {
     public:
        PartIterator(const MimeParser& parent, String_t boundary)
            : m_parent(parent),
              m_boundary(boundary),
              m_position(0)
            {
                // findNextBoundary may fail (return false).
                // This means the message contains multipart headers, but no boundaries.
                // We will treat it as a multipart message with no parts.
                findNextBoundary();
            }

        bool getNextElement(MimeParser& result);

     private:
        bool findNextBoundary();

        const MimeParser& m_parent;
        String_t m_boundary;
        size_t m_position;
    };

    /*
     *  Enumerator implementation that produces the (only) part of an embedded message
     */
    class MessageIterator : public afl::base::Enumerator<MimeParser> {
     public:
        MessageIterator(const MimeParser& parent)
            : m_parent(parent),
              m_did(false)
            { }
        
        bool getNextElement(MimeParser& result);

     private:
        const MimeParser& m_parent;
        bool m_did;
    };

    void copyBodyPart(const MimeParser& from, MimeParser& to, size_t start, size_t end)
    {
        to.clear();
        bool first = true;
        const MimeParser::BodyVec_t& b = from.getBody();
        static const uint8_t NEWLINE[] = {'\n'};
        while (start < end) {
            if (!first) {
                to.handleFullData(NEWLINE);
            }
            to.handleFullData(afl::string::toBytes(b[start]));
            ++start;
            first = false;
        }
        to.finish();
    }
}

/* Read a part.
   \param result [out] Part will be stored here
   \retval true Found another part
   \retval false No more parts; %part has not been changed */
bool
PartIterator::getNextElement(MimeParser& result)
{
    // If user attempts to do stupid things, tell them to go away
    if (&result == &m_parent) {
        return false;
    }

    // Find extent of part
    size_t pos = m_position;
    if (!findNextBoundary()) {
        return false;
    }

    // Decode it
    copyBodyPart(m_parent, result, pos, m_position-1);
    return true;
}

/* Find next boundary. Advances m_position to point after the next boundary (first line of next part).
   \retval true success
   \retval false No more boundaries found */
bool
PartIterator::findNextBoundary()
{
    size_t pos = m_position;
    const MimeParser::BodyVec_t& b = m_parent.getBody();
    while (pos < b.size()) {
        if (b[pos].size() >= m_boundary.size()
            && b[pos].compare(0, m_boundary.size(), m_boundary) == 0
            && b[pos].find_first_not_of('-', m_boundary.size()) == String_t::npos)
        {
            // Line contains boundary, followed by nothing else than "-"'s.
            m_position = pos+1;
            return true;
        }
        ++pos;
    }
    return false;
}


bool
MessageIterator::getNextElement(MimeParser& result)
{
    // If user attempts to do stupid things, tell them to go away
    if (&result == &m_parent) {
        return false;
    }

    // If we already did, we're done
    if (m_did) {
        return false;
    }
    m_did = true;
    
    // Decode it
    copyBodyPart(m_parent, result, 0, m_parent.getBody().size());
    return true;
}

/******************************* MimeParser ******************************/

// Constructor.
afl::net::MimeParser::MimeParser()
    : m_headers(),
      m_trace(),
      m_body(),
      m_accum(),
      m_state(StateHeader),
      m_pLog(0),
      m_headerParser(new HeaderParser(*this))
{
    m_body.push_back(String_t());
}

// Destructor.
afl::net::MimeParser::~MimeParser()
{ }

// Set logger.
void
afl::net::MimeParser::setLog(afl::sys::LogListener* pLog)
{
    m_pLog = pLog;
}

// Add data.
bool
afl::net::MimeParser::handleData(afl::base::ConstBytes_t& data)
{
    while (!data.empty()) {
        bool did;
        switch (m_state) {
         case StateHeader:     addDataHeader(data);     did = true; break;
         case StatePlainBody:  addDataPlainBody(data);  did = true; break;
         case StateBase64Body: addDataBase64Body(data); did = true; break;
         case StateQPBody:     addDataQPBody(data);     did = true; break;
        }
        if (!did) {
            // In place of a 'default' branch.
            // This branch is never hit unless somewhat overwrote the m_state.
            // If it is hit anyway, avoid infinite loop.
            return false;
        }
    }
    return true;
}

// Finish.
void
afl::net::MimeParser::finish()
{
    switch (m_state) {
     case StateHeader: {
        // Two newlines to get it out of the "Header" state
        afl::base::ConstBytes_t bytes(afl::string::toBytes("\n\n"));
        addDataHeader(bytes);
        break;
     }
     case StatePlainBody:
        break;
     case StateBase64Body:
        flushBase64Body();
        break;
     case StateQPBody:
        // Decode pending partial line. Do not handle trailing "=",
        // because there's no line we could join this with.
        addBody(decodeQP(m_accum));
        break;
    }
    m_accum.clear();
}

// Clear.
void
afl::net::MimeParser::clear()
{
    m_headers.clear();
    m_trace.clear();
    m_body.clear();
    m_body.push_back(String_t());
    m_accum.clear();
    m_state = StateHeader;
    m_headerParser.reset(new HeaderParser(*this));
}

// Access headers.
const afl::net::HeaderTable&
afl::net::MimeParser::getHeaders() const
{
    return m_headers;
}

// Access message body.
const afl::net::MimeParser::BodyVec_t&
afl::net::MimeParser::getBody() const
{
    return m_body;
}

// Get header by name.
afl::base::Optional<String_t>
afl::net::MimeParser::getHeader(String_t name) const
{
    if (const HeaderField* hf = m_headers.get(name)) {
        return hf->getValue();
    } else {
        return afl::base::Nothing;
    }
}

// Get file name.
afl::base::Optional<String_t>
afl::net::MimeParser::getFileName() const
{
    // Content-Disposition=>filename
    String_t result;
    if (const HeaderField* hf = m_headers.get("Content-Disposition")) {
        if (hf->getSecondaryValue("filename", result)) {
            return result;
        }
    }

    // Content-Type=>name
    if (const HeaderField* hf = m_headers.get("Content-Type")) {
        if (hf->getSecondaryValue("name", result)) {
            return result;
        }
    }

    return afl::base::Nothing;
}

// Get entire body as string.
String_t
afl::net::MimeParser::getBodyAsString() const
{
    String_t result = m_body[0];
    for (BodyVec_t::size_type i = 1; i < m_body.size(); ++i) {
        result += '\n';
        result += m_body[i];
    }
    return result;
}

// Get trace.
String_t
afl::net::MimeParser::getTrace() const
{
    return m_trace;
}

// Get parts of a multipart message.
afl::base::Ptr<afl::base::Enumerator<MimeParser> >
afl::net::MimeParser::getParts() const
{
    // Check content type
    const HeaderField* hf = m_headers.get("Content-Type");
    if (hf == 0) {
        return 0;
    }

    String_t ct = hf->getPrimaryValue();
    if (afl::string::strCaseCompare(ct, "message/rfc822") == 0) {
        // Forwarded message: body is another message
        return new MessageIterator(*this);
    } else if (afl::string::strCaseCompare(ct.substr(0, 10), "multipart/") == 0) {
        // Multipart: must have a nonempty boundary
        String_t boundary;
        if (!hf->getSecondaryValue("boundary", boundary) || boundary.empty()) {
            return 0;
        }

        // OK, remember it
        return new PartIterator(*this, "--" + boundary);
    } else {
        return 0;
    }
}

// HeaderParser:
void
afl::net::MimeParser::handleHeader(String_t key, String_t value)
{
    if (afl::string::strCaseCompare(key, "Received") == 0) {
        processReceivedHeader(value);
    } else {
        m_headers.add(key, decodeHeader(strLTrim(value), m_pLog));
    }
}

/* Add data while in "Header" state. */
void
afl::net::MimeParser::addDataHeader(afl::base::ConstBytes_t& data)
{
    if (m_headerParser->handleData(data)) {
        // Blank line, we're starting the body
        // processReceivedHeader();
        HeaderField* hf = m_headers.get("Content-Transfer-Encoding");
        if (hf == 0) {
            m_state = StatePlainBody;
        } else {
            String_t typ = strLCase(hf->getPrimaryValue());
            if (typ == "base64") {
                m_state = StateBase64Body;
            } else if (typ == "quoted-printable") {
                m_state = StateQPBody;
            } else {
                if (typ != "8bit" && typ != "7bit") {
                    if (m_pLog != 0) {
                        m_pLog->write(afl::sys::LogListener::Warn, LOG_NAME, Format("unknown Content-Transfer-Encoding '%s' (ignored)", typ));
                    }
                }
                m_state = StatePlainBody;
            }
        }
    }
}

/* Add data while in "Plain Body" state. */
void
afl::net::MimeParser::addDataPlainBody(afl::base::ConstBytes_t& data)
{
    while (const uint8_t* pc = data.eat()) {
        const uint8_t c = *pc;
        if (c == '\r') {
            // skip
        } else if (c == '\n') {
            m_body.push_back(String_t());
        } else {
            m_body.back() += static_cast<char>(c);
        }
    }
}

/* Add data while in "base64 Body" state. */
void
afl::net::MimeParser::addDataBase64Body(afl::base::ConstBytes_t& data)
{
    while (const uint8_t* pc = data.eat()) {
        const uint8_t c = *pc;
        if (c == '\r') {
            // skip
        } else if (c == '\n') {
            // whole line
            flushBase64Body();
        } else {
            m_accum.push_back(c);
        }
    }
}

/* Add data while in "quoted-printable Body" state. */
void
afl::net::MimeParser::addDataQPBody(afl::base::ConstBytes_t& data)
{
    while (const uint8_t* pc = data.eat()) {
        const uint8_t c = *pc;
        if (c == '\r') {
            // skip
        } else if (c == '\n') {
            // whole line
            bool addNL = true;
            if (!m_accum.empty() && m_accum[m_accum.size()-1] == '=') {
                addNL = false;
                m_accum.pop_back();
            }
            addBody(decodeQP(m_accum));
            m_accum.clear();
            if (addNL) {
                m_body.push_back(String_t());
            }
        } else {
            m_accum.push_back(c);
        }
    }
}

/* Add text to the body.
   Handles possible line splitting. */
void
afl::net::MimeParser::addBody(const String_t& str)
{
    for (String_t::size_type i = 0; i < str.size(); ++i) {
        if (str[i] == '\n') {
            m_body.push_back(String_t());
        } else {
            m_body.back() += str[i];
        }
    }
}

/* Finish a base64 part. */
void
afl::net::MimeParser::flushBase64Body()
{
    addBody(decodeBase64(m_accum));
    m_accum.clear();
}

/* Process a "Received" header. */
void
afl::net::MimeParser::processReceivedHeader(const String_t& value)
{
    // Format of a Received line, according to RFC 2821:
    //   Received: "from" <xd> "by" <xd> ["via" <link>] ["with" <proto>] ["id" <string>] ["for" <mail>] ";" <date>
    // where <xd> = <domain> | <domain> "("<addr-literal>")" | <addr-literal>
    //       <addr-literal> = "[" ... "]"
    // We're interested in the address literals only. It seems most servers
    // actually use them in "from", so we just look for the first "[]" block.
    // It needn't be bullet-proof, as it is used for tracing only.
    // FIXME: should this be a functionality of HeaderTable?
    String_t::size_type a = value.find('[');
    String_t::size_type e = value.find(']');
    if (a != String_t::npos && e != String_t::npos && a < e) {
        if (!m_trace.empty()) {
            m_trace.append("/");
        }
        ++a;
        m_trace.append(value, a, e-a);
    }
}
