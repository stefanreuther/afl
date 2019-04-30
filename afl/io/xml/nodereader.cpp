/**
  *  \file afl/io/xml/nodereader.cpp
  *  \brief Class afl::io::xml::NodeReader
  */

#include "afl/io/xml/nodereader.hpp"
#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"
#include "afl/io/xml/visitor.hpp"

/*
 *  Workers
 */

class afl::io::xml::NodeReader::Worker : public afl::base::Deletable {
 public:
    virtual Token readNext(Data& d, WhitespaceMode mode) = 0;
};

class afl::io::xml::NodeReader::TagWorker : public Worker {
 public:
    TagWorker(const TagNode& node)
        : m_node(node),
          m_state(Open),
          m_index(0),
          m_child()
        { }
    virtual Token readNext(Data& d, WhitespaceMode mode);
 private:
    const TagNode& m_node;

    enum State {
        Open,
        Attributes,
        Content,
        Close,
        Done
    };
    State m_state;
    size_t m_index;
    std::auto_ptr<Worker> m_child;
};

class afl::io::xml::NodeReader::PIWorker : public Worker {
 public:
    PIWorker(const PINode& node)
        : m_node(node),
          m_state(Open),
          m_index(0)
        { }
    virtual Token readNext(Data& d, WhitespaceMode mode);
 private:
    const PINode& m_node;

    enum State {
        Open,
        Attributes,
        Done
    };
    State m_state;
    size_t m_index;
};

class afl::io::xml::NodeReader::TextWorker : public Worker {
 public:
    TextWorker(const TextNode& node)
        : m_node(node),
          m_did(false)
        { }
    virtual Token readNext(Data& d, WhitespaceMode mode)
        {
            if (!m_did) {
                d.value = m_node.get();
                m_did = true;

                // Process whitespace trimming
                if (mode != AllWS) {
                    if (d.value.find_first_not_of("\r\n\t ") == std::string::npos) {
                        if (mode == TrimWS) {
                            return Eof;
                        }
                    }
                    d.value = " ";
                }

                return Text;
            } else {
                return Eof;
            }
        }
 private:
    const TextNode& m_node;
    bool m_did;
};

/*
 *  Visitor (=Worker factory)
 */

class afl::io::xml::NodeReader::Visitor : public afl::io::xml::Visitor {
 public:
    Visitor(std::auto_ptr<Worker>& out)
        : m_out(out)
        { }
    virtual void visitPI(const PINode& node)
        { m_out.reset(new PIWorker(node)); }
    virtual void visitTag(const TagNode& node)
        { m_out.reset(new TagWorker(node)); }
    virtual void visitText(const TextNode& node)
        { m_out.reset(new TextWorker(node)); }
 private:
    std::auto_ptr<Worker>& m_out;
};


/*
 *  Worker implementations
 */

afl::io::xml::BaseReader::Token
afl::io::xml::NodeReader::TagWorker::readNext(Data& d, WhitespaceMode mode)
{
    if (m_state == Open) {
        d.tag = m_node.getName();
        m_state = Attributes;
        m_index = 0;
        return TagStart;
    }
    if (m_state == Attributes) {
        if (m_index < m_node.getAttributeNames().getNumNames()) {
            d.tag = m_node.getName();
            d.name = m_node.getAttributeNames().getNameByIndex(m_index);
            d.value = m_node.getAttributeByIndex(m_index);
            ++m_index;
            return TagAttribute;
        } else {
            m_state = Content;
            m_index = 0;
        }
    }
    while (m_state == Content) {
        // Try to read from child
        if (m_child.get() != 0) {
            Token result = m_child->readNext(d, mode);
            if (result != Eof) {
                return result;
            }
            m_child.reset();
        }

        // Try to build a new child
        if (m_index < m_node.getChildren().size()) {
            if (const Node* p = m_node.getChildren()[m_index]) {
                Visitor(m_child).visit(*p);
            }
            ++m_index;
        } else {
            m_state = Close;
        }
    }
    if (m_state == Close) {
        d.tag = m_node.getName();
        m_state = Done;
        return TagEnd;
    }
    return Eof;
}

afl::io::xml::BaseReader::Token
afl::io::xml::NodeReader::PIWorker::readNext(Data& d, WhitespaceMode /*mode*/)
{
    if (m_state == Open) {
        d.tag = m_node.getName();
        m_state = Attributes;
        m_index = 0;
        return PIStart;
    }
    if (m_state == Attributes) {
        if (m_index < m_node.getAttributeNames().getNumNames()) {
            d.tag = m_node.getName();
            d.name = m_node.getAttributeNames().getNameByIndex(m_index);
            d.value = m_node.getAttributeByIndex(m_index);
            ++m_index;
            return PIAttribute;
        } else {
            m_state = Done;
            m_index = 0;
        }
    }
    return Eof;
}



/*
 *  NodeReader
 */

afl::io::xml::NodeReader::NodeReader()
    : m_nodes(),
      m_worker(),
      m_whitespaceMode(AllWS),
      m_data()
{ }

afl::io::xml::NodeReader::~NodeReader()
{ }

void
afl::io::xml::NodeReader::addNode(const Node* pNode)
{
    if (pNode != 0) {
        m_nodes.push_back(pNode);
    }
}

afl::io::xml::BaseReader::Token
afl::io::xml::NodeReader::readNext()
{
    while (1) {
        // Try to read from worker
        if (m_worker.get() != 0) {
            Token t = m_worker->readNext(m_data, m_whitespaceMode);
            if (t != Eof) {
                return t;
            }
            m_worker.reset();
        }

        // That did not work, try to build a new worker
        if (m_nodes.empty()) {
            return Eof;
        }
        Visitor(m_worker).visit(*m_nodes.front());
        m_nodes.erase(m_nodes.begin());
    }
}

String_t
afl::io::xml::NodeReader::getTag() const
{
    return m_data.tag;
}

String_t
afl::io::xml::NodeReader::getName() const
{
    return m_data.name;
}

String_t
afl::io::xml::NodeReader::getValue() const
{
    return m_data.value;
}

void
afl::io::xml::NodeReader::setWhitespaceMode(WhitespaceMode mode)
{
    m_whitespaceMode = mode;
}

afl::io::xml::BaseReader::WhitespaceMode
afl::io::xml::NodeReader::getWhitespaceMode() const
{
    return m_whitespaceMode;
}
