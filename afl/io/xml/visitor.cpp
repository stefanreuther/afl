/**
  *  \file afl/io/xml/visitor.cpp
  *  \brief Interface afl::io::xml::Visitor
  */

#include "afl/io/xml/visitor.hpp"
#include "afl/io/xml/node.hpp"

// Visit a node.
void
afl::io::xml::Visitor::visit(const Node& node)
{
    node.visit(*this);
}

// Visit a node list.
void
afl::io::xml::Visitor::visit(const Nodes_t& nodes)
{
    for (size_t i = 0, n = nodes.size(); i < n; ++i) {
        if (const Node* p = nodes[i]) {
            visit(*p);
        }
    }
}
