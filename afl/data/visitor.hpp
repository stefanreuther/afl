/**
  *  \file afl/data/visitor.hpp
  *  \brief Class afl::data::Visitor
  */
#ifndef AFL_AFL_DATA_VISITOR_HPP
#define AFL_AFL_DATA_VISITOR_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace data {

    class Value;
    class Hash;
    class Vector;

    /** Value visitor.
        This class defines a set of callbacks to process Values depending on their types.
        Values implement a visit() method to implement the type switch.

        Use Visitor::visit() to visit a Value given as pointer. */
    class Visitor : public afl::base::Deletable {
     public:
        /** Constructor. */
        Visitor();

        /** Visit a string.
            Called from StringValue and possibly other string-like values.
            \param str String value */
        virtual void visitString(const String_t& str) = 0;

        /** Visit an integer.
            Called from IntegerValue and possibly other integer-like values.
            \param iv Integer value */
        virtual void visitInteger(int32_t iv) = 0;

        /** Visit a float.
            Called from FloatValue and possibly other float-like values.
            \param fv Float value */
        virtual void visitFloat(double fv) = 0;

        /** Visit a boolean.
            Called from BooleanValue and possibly other boolean-like values.
            \param bv Boolean value */
        virtual void visitBoolean(bool bv) = 0;

        /** Visit a hash.
            Called from HashValue and possibly other hash-like values.
            \param hv Hash value (guaranteed to be heap-allocated) */
        virtual void visitHash(const Hash& hv) = 0;

        /** Visit a vector.
            Called from VectorValue and possibly other vector-like values.
            \param vv Vector value (guaranteed to be heap-allocated) */
        virtual void visitVector(const Vector& vv) = 0;

        /** Visit another value.
            Called from values that don't have a matching other type.
            The implementation might implement a type-switch if required.
            \param other Other value */
        virtual void visitOther(const Value& other) = 0;

        /** Visit null value.
            Called from Visitor::visit when given a null pointer (=null value). */
        virtual void visitNull() = 0;

        /** Visit an error.
            Called from ErrorValue and possibly other error-like values.
            \param source Name of error source
            \param str Error string */
        virtual void visitError(const String_t& source, const String_t& str) = 0;

        /** Visit value.
            When given a value, calls its associated visit method;
            when given a null value, calls visitNull(),
            This is the main entry point.
            \param value Value to examine */
        void visit(Value* value);
    };

} }

inline afl::data::Visitor::Visitor()
{ }

#endif
