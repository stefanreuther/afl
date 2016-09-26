/**
  *  \file afl/io/json/parser.hpp
  *  \brief Class afl::io::json::Parser
  */
#ifndef AFL_AFL_IO_JSON_PARSER_HPP
#define AFL_AFL_IO_JSON_PARSER_HPP

#include "afl/data/valuefactory.hpp"
#include "afl/io/bufferedstream.hpp"

namespace afl { namespace io { namespace json {

    /** JSON Parser.
        Allows reading JSON-formatted data from a BufferedStream.
        The stream must be in UTF-8 encoding (there will be no character translation).
        All JSON data types (numbers, strings, booleans, null, arrays/vectors, objects/hashes) are supported. */
    class Parser {
     public:
        /** Constructor.
            \param stream Stream to read from
            \param factory ValueFactory to create actual objects */
        Parser(afl::io::BufferedStream& stream, afl::data::ValueFactory& factory);

        /** Destructor. */
        ~Parser();

        /** Parse a JSON object.
            Parses one object and leaves the read pointer right behind it.
            Leading whitespace is processed.
            \return parsed object*/
        afl::data::Value* parse();

        /** Parse a JSON object from complete stream.
            Parses one object.
            Makes sure that the stream consists of nothing more than this JSON object (i.e. rejects trailing garbage).
            Leading and trailing whitespace is processed.
            \return parsed object*/
        afl::data::Value* parseComplete();

        /** Skip whitespace.
            Leaves the read pointer at the first non-whitespace character. */
        void skipWhitespace();

     private:
        afl::io::BufferedStream& m_stream;
        afl::data::ValueFactory& m_factory;

        // Error messages
        void endOfFile();
        void syntaxError();

        // Parse individual productions
        afl::data::Value* parseArray();
        afl::data::Value* parseHash();
        afl::data::Value* parseNumber();
        String_t parseString();

        // Parser helpers
        void parseChar(uint8_t which);
        bool parseChar(uint8_t first, uint8_t second);
        uint32_t parseUnicodeEscape();
        void parseKeyword(const char* kw);
    };

} } }


#endif
