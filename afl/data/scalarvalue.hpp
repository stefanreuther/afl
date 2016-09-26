/**
  *  \file afl/data/scalarvalue.hpp
  *  \brief Class afl::data::ScalarValue
  */
#ifndef AFL_AFL_DATA_SCALARVALUE_HPP
#define AFL_AFL_DATA_SCALARVALUE_HPP

#include "afl/data/value.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace data {

    /** Scalar value.
        A value that is internally represented as a 32-bit signed integer.
        It can have various external representations (bool, integer, enum, ...). */
    class ScalarValue : public Value {
     public:
        /** Get value.
            \return Value as 32-bit integer */
        int32_t getValue() const;

     protected:
        /** Constructor.
            \param value Value */
        explicit ScalarValue(int32_t value);

        void setValue(int32_t value);

     private:
        int32_t m_value;
    };

} }

inline int32_t
afl::data::ScalarValue::getValue() const
{
    return m_value;
}

inline
afl::data::ScalarValue::ScalarValue(int32_t value)
    : Value(),
      m_value(value)
{ }

inline void
afl::data::ScalarValue::setValue(int32_t value)
{
    m_value = value;
}

#endif
