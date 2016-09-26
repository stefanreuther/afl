/**
  *  \file afl/data/errorvalue.hpp
  *  \brief Class afl::data::ErrorValue
  */
#ifndef AFL_AFL_DATA_ERRORVALUE_HPP
#define AFL_AFL_DATA_ERRORVALUE_HPP

#include "afl/data/value.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace data {

    /** Error value. Can contain an error message. */
    class ErrorValue : public Value {
     public:
        /** Constructor.
            \param source Name of error source (e.g. file name, network name)
            \param str Text */
        explicit ErrorValue(const String_t& source, const String_t& str);

        virtual ~ErrorValue();

        virtual void visit(Visitor& visitor) const;

        virtual ErrorValue* clone() const;

        const String_t& getText() const;

        const String_t& getSource() const;

     private:
        String_t m_source;
        String_t m_text;
    };

} }

#endif
