/**
  *  \file afl/io/xml/basereader.hpp
  *  \brief Interface afl::io::xml::BaseReader
  */
#ifndef AFL_AFL_IO_XML_BASEREADER_HPP
#define AFL_AFL_IO_XML_BASEREADER_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace io { namespace xml {

    /** XML reader interface.
        This interface defines types and methods for reading XML.
        Derived classes implement the actual parsing.
        The idea is to make it up to the user to make the actual reader as simple or sophisticated as needed
        (namespaces yes/no, external DTDs yes/no, validating yes/no).

        This is a simple tokenizer.
        It does not produce a DOM.
        Users use readNext() to read a token, and then call getTag(), getName(), getValue() to examine it.

        To produce a DOM, see Parser. */
    class BaseReader : public afl::base::Deletable {
     public:
        /** Token type. */
        enum Token {
            /** End of file. */
            Eof,

            /** Starting tag.
                Reports an opening tag.
                - getTag(): tag name */
            TagStart,

            /** Tag attribute.
                Reports an attribute of a tag, immediately after TagStart.
                - getTag(): tag name
                - getName(): attribute name
                - getValue(): attribute value */
            TagAttribute,

            /** Tag end.
                Reports and end (closing) tag.
                - getTag(): tag name */
            TagEnd,

            /** Processing instruction start.
                Reports the start of a processing instruction.
                - getTag(): instruction name */
            PIStart,

            /** Processing instruction attribute.
                Reports a processing instruction attribute, directly after PIStart.
                - getTag(): instruction name
                - getName(): attribute name
                - getValue(): attribute value */
            PIAttribute,

            /** Comment.
                - getValue(): comment content */
            Comment,

            /** Text.
                - getValue(): the text */
            Text,

            /** Null.
                Internal use. */
            Null,

            /** Error.
                Reports an error. */
            Error
        };

        /** Whitespace mode. Determines how tags containing only whitespace are treated. */
        enum WhitespaceMode {
            /** Remove (=don't report) all nodes that contain just whitespace.
                This is the default mode in XML. */
            TrimWS,

            /** Trim nodes that contain just whitespace down to a single space. */
            SingleWS,

            /** Report all whitespace as-is. */
            AllWS
        };

        /** Read next token.
            Reads the token and returns its type.
            The token's properties can then be accessed using getTag(), getName(), getValue() until readNext() is called again.
            \return token type */
        virtual Token readNext() = 0;

        /** Get current tag name.
            For TagStart, TagAttribute, TagEnd, PIStart, PIAttribute, this is the name of the tag or processing instruction.
            For all other token types, this value is undefined.
            \return tag name */
        virtual String_t getTag() const = 0;

        /** Get current attribute name.
            For TagAttribute, PIAttribute, this is the name of the attribute.
            For all other token types, this value is undefined.
            \return attribute name */
        virtual String_t getName() const = 0;

        /** Get current node value.
            For TagAttribute, PIAttribute, this is the value of the attribute.
            For Text, this is the content of the text node.
            In all cases, entity references have been decoded.
            \return value */
        virtual String_t getValue() const = 0;

        /** Set whitespace mode.
            \param mode desired mode */
        virtual void setWhitespaceMode(WhitespaceMode mode) = 0;

        /** Get whitespace mode.
            \return mode */
        virtual WhitespaceMode getWhitespaceMode() const = 0;
    };

} } }

#endif
