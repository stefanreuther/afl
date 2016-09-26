/**
  *  \file afl/data/hashvalue.hpp
  *  \brief Class afl::data::HashValue
  */
#ifndef AFL_AFL_DATA_HASHVALUE_HPP
#define AFL_AFL_DATA_HASHVALUE_HPP

#include "afl/data/value.hpp"
#include "afl/data/hash.hpp"

namespace afl { namespace data {

    /** Value representing a hash.
        The actual hash is contained by-counted-reference, therefore,
        copying a HashValue does not copy the actual hash. */
    class HashValue : public Value {
     public:
        /** Constructor.
            \param pHash Hash object. Must not be null. */
        HashValue(afl::base::Ptr<Hash> pHash);

        /** Destructor. */
        ~HashValue();

        /** Get contained hash object.
            \return Pointer to hash object */
        const afl::base::Ptr<Hash>& getValue() const;

        virtual void visit(Visitor& visitor) const;

        virtual HashValue* clone() const;

     private:
        afl::base::Ptr<Hash> m_pHash;
    };

} }

inline const afl::base::Ptr<afl::data::Hash>&
afl::data::HashValue::getValue() const
{
    return m_pHash;
}

#endif
