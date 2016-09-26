/**
  *  \file afl/data/integervalue.hpp
  *  \brief Class afl::data::IntegerValue
  */
#ifndef AFL_AFL_DATA_INTEGERVALUE_HPP
#define AFL_AFL_DATA_INTEGERVALUE_HPP

#include "afl/data/scalarvalue.hpp"

namespace afl { namespace data {

    /** Integer value. A scalar value that can hold the whole 32-bit integer range. */
    class IntegerValue : public ScalarValue {
     public:
        /** Constructor.
            \param value Value */
        explicit IntegerValue(int32_t value);

        ~IntegerValue();

        virtual void visit(Visitor& visitor) const;

        virtual IntegerValue* clone() const;

        void add(int32_t delta);
    };

} }

#endif
