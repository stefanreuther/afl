/**
  *  \file afl/io/xml/writer.hpp
  *  \brief Class afl::io::xml::Writer
  */
#ifndef AFL_AFL_IO_XML_WRITER_HPP
#define AFL_AFL_IO_XML_WRITER_HPP

#include "afl/io/xml/visitor.hpp"
#include "afl/io/datasink.hpp"

namespace afl { namespace io { namespace xml {

    /** Simple XML writer.
        A visitor that serializes all nodes it visits into a DataSink.
        This can be used to save a set of nodes that can later be parsed again
        using a Reader and a DefaultEntityHandler. */
    class Writer : public Visitor {
     public:
        /** Constructor.
            \param sink Target sink. Lifetime must exceed that of the Writer */
        explicit Writer(DataSink& sink);
        ~Writer();

        // Visitor:
        virtual void visitPI(const PINode& node);
        virtual void visitTag(const TagNode& node);
        virtual void visitText(const TextNode& node);

        /** Escape special characters.
            Replaces less-than, greater-than, apersand and double-quote by entity references.
            \param in String to escape
            \return result */
        static String_t escape(const String_t& in);

        /** Get inner text of a node.
            \param node Node
            \return text */
        static String_t getInnerText(const Node& node);

     private:
        DataSink& m_sink;

        template<typename NodeT>
        void writeAttributes(const NodeT& node);
    };

} } }

#endif
