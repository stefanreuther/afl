/**
  *  \file afl/io/xml/parser.hpp
  *  \brief Class afl::io::xml::Parser
  */
#ifndef AFL_AFL_IO_XML_PARSER_HPP
#define AFL_AFL_IO_XML_PARSER_HPP

#include "afl/io/xml/basereader.hpp"
#include "afl/io/xml/node.hpp"

namespace afl { namespace io { namespace xml {

    /** Parser and DOM builder.
        Reads the token stream of a BaseReader and builds a tree of Node objects.

        <b>Note:</b>
        This parser works with a lookahead of one token.
        Upon construction, it will read the first token from the BaseReader to seed the lookahead.
        After parse(), it will have read one token beyond the returned node.

        Therefore, if you wish to read multiple nodes, you must re-use the Parser instance
        or you lose tokens. */
    class Parser {
     public:
        /** Constructor.
            \param rdr Reader providing a stream of tokens. Must live longer than Parser. */
        explicit Parser(BaseReader& rdr);

        /** Destructor. */
        ~Parser();

        /** Read one node.
            Reads the node and its content.
            Comments and attributes of a possible surrounding tag are ignored.
            A closing tag causes null to be returned.

            \return Newly-allocated node; caller must take ownership. Null on error/end. */
        Node* parse();

        /** Read multiple nodes.
            Reads nodes until an error or a closing tag is encountered.
            \param nodes [in/out] Nodes */
        void parseNodes(Nodes_t& nodes);

     private:
        Node* parseTag();
        Node* parsePI();
        void readNext();

        /// BaseReader instance.
        BaseReader& m_reader;

        /// Current token (lookahead).
        BaseReader::Token m_token;
    };

} } }

#endif
