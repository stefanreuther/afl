/**
  *  \file afl/net/redis/stringlistkey.hpp
  *  \brief Class afl::net::redis::StringListKey
  */
#ifndef AFL_AFL_NET_REDIS_STRINGLISTKEY_HPP
#define AFL_AFL_NET_REDIS_STRINGLISTKEY_HPP

#include "afl/net/redis/listkey.hpp"
#include "afl/data/stringlist.hpp"

namespace afl { namespace net { namespace redis {

    /** String list key.
        Represents a list of string values.

        Lists have a beginning (left, front end) and an end (right, back end).

        Positive indexes count from the beginning (0=first, 1=second, etc.).
        Negative indexes count from the end (-1=last, -2=penultimate, etc.). */
    class StringListKey : public ListKey {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        StringListKey(CommandHandler& ch, String_t name);

        /** Get element by index (LINDEX).
            \param index Index
            \return Element if found, "" if none */
        String_t operator[](int32_t index) const;

        /** Extract first element (LPOP).
            Removes and returns the element.
            \return Element value if found, "" if none */
        String_t popFront();

        /** Insert element at beginning (LPUSH).
            \param value Element to insert
            \return New number of elements */
        int32_t pushFront(String_t value);

        /** Extract last element (RPOP).
            Removes and returns the element.
            \return Element value if found, "" if none */
        String_t popBack();

        /** Insert element at end (RPUSH).
            \param value Element to insert
            \return New number of elements */
        int32_t pushBack(String_t value);

        /** Set element at index (LSET).
            Replaces an existing element's value.
            \param index Index
            \param value Value to set */
        void set(int32_t index, String_t value);

        /** Remove elements by value (LREM).
            For count>0, removes \c count matching elements, starting at the beginning.
            For count<0, removes \c -count matching elements, starting at the end.
            For count=0, removes all matching elements.
            \param value Value to remove
            \param count Limit
            \return Number of elements removed */
        int32_t removeValue(String_t value, int32_t count);

        /** Transfer element to another list (RPOPLPUSH).
            Removes the last element of this list (like popBack()) and prepends it to the other one (like pushFront()).
            \param other Other list
            \return Value of the transferred element */
        String_t transferElement(StringListKey& other);

        /** Get subrange of list (LRANGE).
            Gets the value of all elements in a given index range.
            \param start [in] Index of first element to extract (inclusive)
            \param end   [in] Index of last element to extract (inclusive)
            \param list  [out] List elements are appended here */
        void getRange(int32_t start, int32_t end, afl::data::StringList_t& list) const;

        /** Get all elements of the list (LRANGE).
            \param list  [out] List elements are appended here */
        void getAll(afl::data::StringList_t& list) const;
    };

} } }


#endif
