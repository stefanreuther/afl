/**
  *  \file afl/data/stringvalue.hpp
  *  \brief Class afl::data::StringValue
  */
#ifndef AFL_AFL_DATA_STRINGVALUE_HPP
#define AFL_AFL_DATA_STRINGVALUE_HPP

#include "afl/data/value.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace data {

    /** String value. A value that holds a string (as copy). */
    class StringValue : public Value {
     public:
        /** Constructor.
            \param value Value */
        StringValue(const String_t& value);

        ~StringValue();

        /** Get value.
            \return String value */
        const String_t& getValue() const;

        virtual void visit(Visitor& visitor) const;

        virtual StringValue* clone() const;

     private:
        String_t m_value;
    };

} }

inline const String_t&
afl::data::StringValue::getValue() const
{
    return m_value;
}


#endif
