/**
  *  \file afl/data/vectorvalue.hpp
  *  \brief Class afl::data::VectorValue
  */
#ifndef AFL_AFL_DATA_VECTORVALUE_HPP
#define AFL_AFL_DATA_VECTORVALUE_HPP

#include "afl/data/value.hpp"
#include "afl/data/vector.hpp"

namespace afl { namespace data {

    /** Value representing a vector.
        The actual vector is contained by-counted-reference, therefore,
        copying a VectorValue does not copy the actual vector. */
    class VectorValue : public Value {
     public:
        /** Constructor.
            \param pVector Vector object. Must not be null. */
        VectorValue(afl::base::Ptr<Vector> pVector);

        /** Destructor. */
        ~VectorValue();

        /** Get contained vector object.
            \return Pointer to vector object */
        const afl::base::Ptr<Vector>& getValue() const;

        virtual void visit(Visitor& visitor) const;

        virtual VectorValue* clone() const;

     private:
        afl::base::Ptr<Vector> m_pVector;
    };

} }

inline const afl::base::Ptr<afl::data::Vector>&
afl::data::VectorValue::getValue() const
{
    return m_pVector;
}

#endif
