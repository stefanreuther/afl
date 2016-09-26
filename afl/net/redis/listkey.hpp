/**
  *  \file afl/net/redis/listkey.hpp
  *  \brief Class afl::net::redis::ListKey
  */
#ifndef AFL_AFL_NET_REDIS_LISTKEY_HPP
#define AFL_AFL_NET_REDIS_LISTKEY_HPP

#include "afl/net/redis/key.hpp"

namespace afl { namespace net { namespace redis {

    class SortOperation;

    /** List key.
        Objects of this type allow generic operations on a list,
        derived classes allow typed access to the list elements.

        Lists have a beginning (left, front end) and an end (right, back end).

        Positive indexes count from the beginning (0=first, 1=second, etc.).
        Negative indexes count from the end (-1=last, -2=penultimate, etc.). */
    class ListKey : public Key {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        ListKey(CommandHandler& ch, String_t name);

        /** Get number of elements in list (LLEN).
            \return Number of elements (signed to be compatible with indexes) */
        int32_t size() const;

        /** Check emptiness (LLEN).
            \return true iff this list is empty */
        bool empty() const;

        /** Trim list to a given range (LTRIM).
            Removes all elements before \c start, and all elements after \c end.
            \param start [in] Index of first element to keep (inclusive)
            \param end   [in] Index of last element to keep (inclusive) */
        void trimToRange(int32_t start, int32_t end);

        /** Sort operation (SORT).
            Creates s SortOperation to sort the elements of this list.
            Sort order, keys, and return values are configured on the SortOperation. */
        SortOperation sort() const;
    };

} } }

#endif
