/**
  *  \file afl/net/redis/integerlistkey.hpp
  *  \brief Class afl::net::redis::IntegerList
  */
#ifndef AFL_AFL_NET_REDIS_INTEGERLISTKEY_HPP
#define AFL_AFL_NET_REDIS_INTEGERLISTKEY_HPP

#include "afl/net/redis/listkey.hpp"
#include "afl/data/integerlist.hpp"

namespace afl { namespace net { namespace redis {

    /** Integer list key.
        Represents a list of numeric values.

        Lists have a beginning (left, front end) and an end (right, back end).

        Positive indexes count from the beginning (0=first, 1=second, etc.).
        Negative indexes count from the end (-1=last, -2=penultimate, etc.).

        Note that although redis can handle arbitrarily large numbers, this class is limited to 32-bit signed integers.
        Values outside this range will produce a InvalidDataException when queried. */
    class IntegerListKey : public ListKey {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        IntegerListKey(CommandHandler& ch, String_t name);

        /** Get element by index (LINDEX).
            \param index Index
            \return Element if found, 0 if none */
        int32_t operator[](int32_t index) const;

        /** Extract first element (LPOP).
            Removes and returns the element.
            \return Element value if found, 0 if none */
        int32_t popFront();

        /** Insert element at beginning (LPUSH).
            \param value Element to insert
            \return New number of elements */
        int32_t pushFront(int32_t value);

        /** Extract last element (RPOP).
            Removes and returns the element.
            \return Element value if found, 0 if none */
        int32_t popBack();

        /** Insert element at end (RPUSH).
            \param value Element to insert
            \return New number of elements */
        int32_t pushBack(int32_t value);

        /** Set element at index (LSET).
            Replaces an existing element's value.
            \param index Index
            \param value Value to set */
        void set(int32_t index, int32_t value);

        /** Remove elements by value (LREM).
            For count>0, removes \c count matching elements, starting at the beginning.
            For count<0, removes \c -count matching elements, starting at the end.
            For count=0, removes all matching elements.
            \param value Value to remove
            \param count Limit
            \return Number of elements removed */
        int32_t removeValue(int32_t value, int32_t count);

        /** Transfer element to another list (RPOPLPUSH).
            Removes the last element of this list (like popBack()) and prepends it to the other one (like pushFront()).
            \param other Other list
            \return Value of the transferred element */
        int32_t transferElement(IntegerListKey& other);

        /** Get subrange of list (LRANGE).
            Gets the value of all elements in a given index range.
            \param start [in] Index of first element to extract (inclusive)
            \param end   [in] Index of last element to extract (inclusive)
            \param list  [out] List elements are appended here */
        void getRange(int32_t start, int32_t end, afl::data::IntegerList_t& list) const;

        /** Get all elements of the list (LRANGE).
            \param list  [out] List elements are appended here */
        void getAll(afl::data::IntegerList_t& list) const;
    };

} } }

#endif
