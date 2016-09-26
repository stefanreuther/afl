/**
  *  \file afl/net/redis/integersetkey.hpp
  *  \brief Class afl::net::redis::IntegerSetKey
  */
#ifndef AFL_AFL_NET_REDIS_INTEGERSETKEY_HPP
#define AFL_AFL_NET_REDIS_INTEGERSETKEY_HPP

#include "afl/net/redis/setkey.hpp"
#include "afl/data/integerlist.hpp"

namespace afl { namespace net { namespace redis {

    class IntegerSetOperation;

    /** Set of integers.
        Describes a set of integers and operations on them. */
    class IntegerSetKey : public SetKey {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        IntegerSetKey(CommandHandler& ch, const String_t& name);

        // Re-import remove() to remove the whole key
        using Key::remove;

        /** Add value to the set (SADD).
            \param value Value to add
            \retval true element was added
            \retval false element was already in the set */
        bool add(int32_t value);

        /** Remove a value from the set (SREM).
            \param value Value to remove
            \retval true element was removed
            \retval false element was not in the set */
        bool remove(int32_t value);

        /** Extract a random value (SPOP).
            Picks a random element, removes it from the set, and returns it.
            \return extracted element; 0 if list was empty */
        int32_t extractRandom();

        /** Get a random value (SRANDMEMBER).
            Picks a random element and returns it.
            Does not modify the set.
            \return selected element; 0 if list was empty */
        int32_t getRandom() const;

        /** Move a value into another set (SMOVE).
            It is not an error if the receiving set already contains the element.
            \param value Value to move
            \param other Receiving set
            \retval true element was moved; this set no longer contains it, \c other contains it
            \retval false element was not contained in this set; both sets unmodified */
        bool moveTo(int32_t value, const IntegerSetKey& other);

        /** Check whether set contains an element (SISMEMBER).
            \param value Element to check
            \return true if element is member of set */
        bool contains(int32_t value) const;

        /** Copy whole set (SINTER).
            Performs a server-side copy of the whole set.
            \param other Receiving set */
        void copyTo(const IntegerSetKey& other);

        /** Get all elements (SMEMBERS).
            Order will be unspecified; use sort() to define an order.
            \param list [out] Set elements will be appended here */
        void getAll(afl::data::IntegerList_t& list) const;

        /** Compute intersection of sets (SINTER, SINTERSTORE).
            The intersection contains all elements that are member of all participating sets.
            Use IntegerSetOperation's members to intersect more sets and obtain the result.
            \param other Other set to intersect
            \return set operation */
        IntegerSetOperation intersect(const IntegerSetKey& other) const;

        /** Compute union of sets (SUNION, SUNIONSTORE).
            The union contains all elements that are member of at least one participating set.
            Use IntegerSetOperation's members to merge more sets and obtain the result.
            \param other Other set to merge
            \return set operation */
        IntegerSetOperation merge(const IntegerSetKey& other) const;

        /** Compute difference of sets (SDIFF, SDIFFSTORE).
            The difference contains all elements that are member of the first, but not of any of the following sets.
            Use IntegerSetOperation's members to subtract more sets and obtain the result.
            \param other Other set to subtract
            \return set operation */
        IntegerSetOperation remove(const IntegerSetKey& other) const;
    };

} } }

#endif
