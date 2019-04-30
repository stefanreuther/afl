/**
  *  \file afl/io/xml/textnode.hpp
  *  \brief Class afl::io::xml::TextNode
  */
#ifndef AFL_AFL_IO_XML_TEXTNODE_HPP
#define AFL_AFL_IO_XML_TEXTNODE_HPP

#include "afl/io/xml/node.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace io { namespace xml {

    /** Text node.
        Represents a piece of text (BaseReader::Text). */
    class TextNode : public Node {
     public:
        /** Constructor.
            Makes a blank node. */
        TextNode();

        /** Constructor.
            Makes a node containing the given text.
            \param value Text */
        TextNode(const String_t& value);

        /** Destructor. */
        ~TextNode();

        /** Set contained text.
            \param newValue New value */
        void set(const String_t& newValue);

        /** Get contained text.
            \return text */
        const String_t& get() const;

        // Node:
        virtual void visit(Visitor& v) const;

     private:
        String_t m_value;
    };

} } }

#endif
