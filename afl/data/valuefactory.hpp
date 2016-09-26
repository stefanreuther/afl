/**
  *  \file afl/data/valuefactory.hpp
  *  \brief Interface afl::data::ValueFactory
  */
#ifndef AFL_AFL_DATA_VALUEFACTORY_HPP
#define AFL_AFL_DATA_VALUEFACTORY_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/types.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace data {

    class NameMap;
    class Segment;
    class Value;

    /** Interface for creating values.
        This interface is used by deserializers and similar functions
        to allow users to determine the actual types created by deserializing.

        Return values always are newly-allocated (or null) objects. */
    class ValueFactory : public afl::base::Deletable {
     public:
        /** Create string value.
            \param sv String
            \return value */
        virtual Value* createString(const String_t& sv) = 0;

        /** Create integer value.
            \param iv Integer
            \return value */
        virtual Value* createInteger(int32_t iv) = 0;

        /** Create float value.
            \param fv Float
            \return value */
        virtual Value* createFloat(double fv) = 0;

        /** Create boolean value.
            \param bv Boolean
            \return value */
        virtual Value* createBoolean(bool bv) = 0;

        /** Create hash value.
            \param keys Keys (can be modified, i.e. swapped)
            \param values Values (can be modified, i.e. swapped)
            \return value */
        virtual Value* createHash(NameMap& keys, Segment& values) = 0;

        /** Create vector value.
            \param values Values (can be modified, i.e. swapped)
            \return value */
        virtual Value* createVector(Segment& values) = 0;

        /** Create error value.
            \param source Name of error source
            \param str Text
            \return value */
        virtual Value* createError(const String_t& source, const String_t& str) = 0;

        /** Create null value.
            \return value (probably null) */
        virtual Value* createNull() = 0;
    };

} }

#endif
