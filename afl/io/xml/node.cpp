/**
  *  \file afl/io/xml/node.cpp
  */

#include "afl/io/xml/node.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"
#include "afl/io/xml/visitor.hpp"

String_t
afl::io::xml::Node::getTextContent() const
{
    class V : public afl::io::xml::Visitor {
     public:
        virtual void visitPI(const PINode&)
            { }
        virtual void visitText(const TextNode& t)
            { m_result += t.get(); }
        virtual void visitTag(const TagNode& t)
            { visit(t.getChildren()); }
        const String_t& get()
            { return m_result; }
     private:
        String_t m_result;
    };
    V v;
    v.visit(*this);
    return v.get();
}
