/**
  *  \file afl/data/defaultvaluefactory.hpp
  *  \brief Class afl::data::DefaultValueFactory
  */
#ifndef AFL_AFL_DATA_DEFAULTVALUEFACTORY_HPP
#define AFL_AFL_DATA_DEFAULTVALUEFACTORY_HPP

#include "afl/data/valuefactory.hpp"

namespace afl { namespace data {

    /** Default value factory. Creates regular, unmodified value objects. */
    class DefaultValueFactory : public ValueFactory {
     public:
        DefaultValueFactory();
        ~DefaultValueFactory();

        // ValueFactory methods:
        virtual Value* createString(const String_t& sv);
        virtual Value* createInteger(int32_t iv);
        virtual Value* createFloat(double fv);
        virtual Value* createBoolean(bool bv);
        virtual Value* createHash(NameMap& keys, Segment& values);
        virtual Value* createVector(Segment& values);
        virtual Value* createError(const String_t& source, const String_t& str);
        virtual Value* createNull();
    };

} }

#endif
