/**
  *  \file afl/net/redis/stringsetkey.hpp
  *  \brief Class afl::net::redis::StringSetKey
  */
#ifndef AFL_AFL_NET_REDIS_STRINGSETKEY_HPP
#define AFL_AFL_NET_REDIS_STRINGSETKEY_HPP

#include "afl/net/redis/setkey.hpp"
#include "afl/data/stringlist.hpp"

namespace afl { namespace net { namespace redis {

    class StringSetOperation;

    /** Set of strings.
        Describes a set of strings and operations on them. */
    class StringSetKey : public SetKey {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        StringSetKey(CommandHandler& ch, const String_t& name);

        // Re-import remove() to remove the whole key
        using Key::remove;

        /** Add value to the set (SADD).
            \param value Value to add
            \retval true element was added
            \retval false element was already in the set */
        bool add(String_t value);

        /** Remove a value from the set (SREM).
            \param value Value to remove
            \retval true element was removed
            \retval false element was not in the set */
        bool remove(String_t value);

        /** Extract a random value (SPOP).
            Picks a random element, removes it from the set, and returns it.
            \return extracted element; 0 if list was empty */
        String_t extractRandom();

        /** Get a random value (SRANDMEMBER).
            Picks a random element and returns it.
            Does not modify the set.
            \return selected element; 0 if list was empty */
        String_t getRandom() const;

        /** Move a value into another set (SMOVE).
            It is not an error if the receiving set already contains the element.
            \param value Value to move
            \param other Receiving set
            \retval true element was moved; this set no longer contains it, \c other contains it
            \retval false element was not contained in this set; both sets unmodified */
        bool moveTo(String_t value, const StringSetKey& other);

        /** Check whether set contains an element (SISMEMBER).
            \param value Element to check
            \return true if element is member of set */
        bool contains(String_t value) const;

        /** Copy whole set (SINTER).
            Performs a server-side copy of the whole set.
            \param other Receiving set */
        void copyTo(const StringSetKey& other);

        /** Get all elements (SMEMBERS).
            Order will be unspecified; use sort() to define an order.
            \param list [out] Set elements will be appended here */
        void getAll(afl::data::StringList_t& list) const;

        /** Compute intersection of sets (SINTER, SINTERSTORE).
            The intersection contains all elements that are member of all participating sets.
            Use StringSetOperation's members to intersect more sets and obtain the result.
            \param other Other set to intersect
            \return set operation */
        StringSetOperation intersect(const StringSetKey& other) const;

        /** Compute union of sets (SUNION, SUNIONSTORE).
            The union contains all elements that are member of at least one participating set.
            Use StringSetOperation's members to merge more sets and obtain the result.
            \param other Other set to merge
            \return set operation */
        StringSetOperation merge(const StringSetKey& other) const;

        /** Compute difference of sets (SDIFF, SDIFFSTORE).
            The difference contains all elements that are member of the first, but not of any of the following sets.
            Use StringSetOperation's members to subtract more sets and obtain the result.
            \param other Other set to subtract
            \return set operation */
        StringSetOperation remove(const StringSetKey& other) const;
    };

} } }

#endif
