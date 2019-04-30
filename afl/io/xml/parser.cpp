/**
  *  \file afl/io/xml/parser.cpp
  *  \brief Class afl::io::xml::Parser
  */

#include <memory>
#include "afl/io/xml/parser.hpp"
#include "afl/io/xml/textnode.hpp"
#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/tagnode.hpp"

afl::io::xml::Parser::Parser(BaseReader& rdr)
    : m_reader(rdr),
      m_token(BaseReader::Null)
{
    readNext();
}

afl::io::xml::Parser::~Parser()
{ }

afl::io::xml::Node*
afl::io::xml::Parser::parse()
{
    std::auto_ptr<Node> result;
    bool loop;
    do {
        loop = false;
        switch (m_token) {
         case BaseReader::Eof:
            // Nothing to do
            break;

         case BaseReader::TagStart:
            // Regular tag
            result.reset(parseTag());
            break;

         case BaseReader::TagEnd:
            // Tag end; nothing to do. Caller deals with it.
            break;

         case BaseReader::PIStart:
            // PI
            result.reset(parsePI());
            break;

         case BaseReader::TagAttribute:
         case BaseReader::PIAttribute:
         case BaseReader::Comment:
            // Attributes cannot appear here: skip
            // Comments are not preserved: skip
            loop = true;
            readNext();
            break;

         case BaseReader::Text:
            // Text
            result.reset(new TextNode(m_reader.getValue()));
            readNext();
            break;

         case BaseReader::Null:
         case BaseReader::Error:
            // Error; nothing to do
            break;
        }
    } while (loop);
    return result.release();
}

void
afl::io::xml::Parser::parseNodes(Nodes_t& nodes)
{
    while (Node* pNode = parse()) {
        nodes.pushBackNew(pNode);
    }
}

afl::io::xml::Node*
afl::io::xml::Parser::parseTag()
{
    // FIXME: limit recursion depth
    // Build node
    std::auto_ptr<TagNode> result(new TagNode(m_reader.getTag()));
    readNext();

    // Read attributes
    while (m_token == BaseReader::TagAttribute) {
        result->setAttribute(m_reader.getName(), m_reader.getValue());
        readNext();
    }

    // Read content
    parseNodes(result->getChildren());

    // Validate
    if (m_token == BaseReader::TagEnd && m_reader.getTag() == result->getName()) {
        readNext();
    }

    return result.release();
}

afl::io::xml::Node*
afl::io::xml::Parser::parsePI()
{
    // Build node
    std::auto_ptr<PINode> result(new PINode(m_reader.getTag()));
    readNext();

    // Read attributes
    while (m_token == BaseReader::PIAttribute) {
        result->setAttribute(m_reader.getName(), m_reader.getValue());
        readNext();
    }
    return result.release();
}

void
afl::io::xml::Parser::readNext()
{
    m_token = m_reader.readNext();
}
