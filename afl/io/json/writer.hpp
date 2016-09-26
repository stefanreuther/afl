/**
  *  \file afl/io/json/writer.hpp
  *  \brief Class afl::io::json::Writer
  */
#ifndef AFL_AFL_IO_JSON_WRITER_HPP
#define AFL_AFL_IO_JSON_WRITER_HPP

#include "afl/data/visitor.hpp"
#include "afl/io/datasink.hpp"

namespace afl { namespace io { namespace json {

    class Stream;

    /** JSON writer. Formats data into JSON text. */
    class Writer : public afl::data::Visitor {
     public:
        /** Constructor.
            \param sink Data sink to write to. Using buffering is recommended. */
        Writer(afl::io::DataSink& sink);

        /** Destructor. */
        ~Writer();

        /** Set line length limit.
            Output lines will be limited to around this size.
            (It is not an absolute limit and will be exceeded from time to time.)
            \param lineLength Line length limit in bytes. Default is 0 meaning no limit. */
        void setLineLength(size_t lineLength);

        /** Set indentation step.
            If set to a nonzero value, hashes and vectors will be formatted as multiple lines,
            each level indented by this many spaces.
            \param indentStep Indentation level */
        void setIndentStep(size_t indentStep);

        // Visitor:
        virtual void visitString(const String_t& str);
        virtual void visitInteger(int32_t iv);
        virtual void visitFloat(double fv);
        virtual void visitBoolean(bool bv);
        virtual void visitHash(const afl::data::Hash& hv);
        virtual void visitVector(const afl::data::Vector& vv);
        virtual void visitOther(const afl::data::Value& other);
        virtual void visitError(const String_t& source, const String_t& str);
        virtual void visitNull();

     private:
        // Configuration:
        size_t m_lineLength;
        size_t m_indentStep;
        afl::io::DataSink& m_sink;

        // State:
        size_t m_currentLineLength;
        size_t m_currentIndentLevel;
        bool m_currentAllowNewline;

        static const uint32_t NewlineBefore = 1;
        static const uint32_t NewlineAfter = 2;
        static const uint32_t IndentAfter = 4;
        static const uint32_t UndentBefore = 8;

        void write(const String_t& str, uint32_t flags);
    };

} } }

#endif
