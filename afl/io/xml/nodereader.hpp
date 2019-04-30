/**
  *  \file afl/io/xml/nodereader.hpp
  *  \brief Class afl::io::xml::NodeReader
  */
#ifndef AFL_AFL_IO_XML_NODEREADER_HPP
#define AFL_AFL_IO_XML_NODEREADER_HPP

#include <vector>
#include <memory>
#include "afl/io/xml/basereader.hpp"
#include "afl/io/xml/node.hpp"

namespace afl { namespace io { namespace xml {

    /** Read a Node tree (DOM) as token sequence.
        Provides a token sequence that, when given to a Parser, reproduces a copy of the given Node tree.

        To use, construct a NodeReader and call addNode() for all nodes you wish to reproduce.
        Then call the BaseReader methods to obtain the token sequence.
        The token sequence ends with an Eof token; if you keep calling readNext(),
        NodeReader returns Eof until more nodes are added using addNode(). */
    class NodeReader : public BaseReader {
     public:
        /** Constructor.
            Makes an empty NodeReader. */
        NodeReader();

        /** Destructor. */
        ~NodeReader();

        /** Add node to reproduce.
            Nodes queue up; the new node will be reproduced after all previously added nodes are finished.

            \param pNode Node. Null values are ignored. The node remains owned by the caller and needs to
                         have sufficient lifetime (until NodeReader reports Eof) */
        void addNode(const Node* pNode);

        // BaseReader:
        virtual Token readNext();
        virtual String_t getTag() const;
        virtual String_t getName() const;
        virtual String_t getValue() const;
        virtual void setWhitespaceMode(WhitespaceMode mode);
        virtual WhitespaceMode getWhitespaceMode() const;

     private:
        class Worker;
        class TagWorker;
        class PIWorker;
        class TextWorker;
        class Visitor;

        std::vector<const Node*> m_nodes;
        std::auto_ptr<Worker> m_worker;

        WhitespaceMode m_whitespaceMode;

        struct Data {
            String_t tag;
            String_t name;
            String_t value;
        };
        Data m_data;
    };

} } }

#endif
