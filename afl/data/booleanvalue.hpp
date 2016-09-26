/**
  *  \file afl/data/booleanvalue.hpp
  *  \brief Class afl::data::BooleanValue
  */
#ifndef AFL_AFL_DATA_BOOLEANVALUE_HPP
#define AFL_AFL_DATA_BOOLEANVALUE_HPP

#include "afl/data/scalarvalue.hpp"

namespace afl { namespace data {

    /** Boolean value. A scalar value that can take values 0 (false) or 1 (true). */
    class BooleanValue : public ScalarValue {
     public:
        /** Constructor.
            \param value Value */
        explicit BooleanValue(bool value);

        ~BooleanValue();

        virtual void visit(Visitor& visitor) const;

        virtual BooleanValue* clone() const;
    };

} }

#endif
