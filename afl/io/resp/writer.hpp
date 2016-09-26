/**
  *  \file afl/io/resp/writer.hpp
  *  \brief Class afl::io::resp::Writer
  */
#ifndef AFL_AFL_IO_RESP_WRITER_HPP
#define AFL_AFL_IO_RESP_WRITER_HPP

#include "afl/data/visitor.hpp"
#include "afl/io/datasink.hpp"
#include "afl/data/segment.hpp"

namespace afl { namespace io { namespace resp {

    /** RESP writer.
        Formats data into RESP (Redis Serialisation Protocol) text.

        Because Redis only accepts arrays-of-bulk-string, we produce only that (for non-null data).
        \todo Make an option to produce actual integers? */
    class Writer : public afl::data::Visitor {
     public:
        /** Constructor.
            \param sink Data sink to write to. Using buffering is recommended. */
        Writer(afl::io::DataSink& sink);

        /** Destructor. */
        ~Writer();

        // Local interface:

        /** Send an error message.
            The error message is received as a RemoteErrorException by Parser.
            \param str error message, must not contain control characters nor end in a newline */
        void sendError(const String_t& str);

        /** Send a success message.
            The success message is received as a string object by Parser.
            \param str success message, must not contain control characters nor end in a newline */
        void sendSuccess(const String_t& str);

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

        // FIXME:
        void visitSegment(const afl::data::Segment& seg);

     private:
        afl::io::DataSink& m_sink;

        void write(afl::string::ConstStringMemory_t str);
    };

} } }

#endif
