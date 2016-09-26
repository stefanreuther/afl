/**
  *  \file afl/data/value.hpp
  *  \brief Class afl::data::Value
  */
#ifndef AFL_AFL_DATA_VALUE_HPP
#define AFL_AFL_DATA_VALUE_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/clonable.hpp"

namespace afl { namespace data {

    class Visitor;

    /** Boxed value.
        This is the base type for immutable boxed values.

        To check for a specific value type, use dynamic_cast.
        For bulk type switches, use Visitor.

        Values are immutable by default.
        By convention, values have strict ownership semantics,
        i.e. are referenced by exactly one auto_ptr or PtrVector.
        This allows mutable values as an optimisation. */
    class Value : public afl::base::Deletable,
                  public afl::base::Clonable<Value>
    {
     public:
        /** Visit this value.
            Must call exactly one method of Visitor with appropriate parameters.
            \param visitor Visitor */
        virtual void visit(Visitor& visitor) const = 0;
    };

} }

#endif
