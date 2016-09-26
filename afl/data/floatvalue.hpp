/**
  *  \file afl/data/floatvalue.hpp
  *  \brief Class afl::data::FloatValue
  */
#ifndef AFL_AFL_DATA_FLOATVALUE_HPP
#define AFL_AFL_DATA_FLOATVALUE_HPP

#include "afl/data/value.hpp"

namespace afl { namespace data {

    /** Floating-point value. Represented as a C++ double. */
    class FloatValue : public Value {
     public:
        /** Constructor.
            \param value Value */
        explicit FloatValue(double value);

        ~FloatValue();

        /** Get value.
            Returns the value this was constructed with. */
        double getValue() const;

        virtual void visit(Visitor& visitor) const;

        virtual FloatValue* clone() const;

        void add(double delta);

     private:
        double m_value;
    };

} }

inline double
afl::data::FloatValue::getValue() const
{
    return m_value;
}

#endif
