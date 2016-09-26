/**
  *  \file afl/net/redis/setkey.hpp
  *  \brief Class afl::net::redis::SetKey
  */
#ifndef AFL_AFL_NET_REDIS_SETKEY_HPP
#define AFL_AFL_NET_REDIS_SETKEY_HPP

#include "afl/net/redis/key.hpp"

namespace afl { namespace net { namespace redis {

    class SortOperation;

    /** List key.
        Objects of this type allow generic operations on a set,
        derived classes allow typed access to the set elements. */
    class SetKey : public Key {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        SetKey(CommandHandler& ch, const String_t& name);

        /** Get number of elements in set (SCARD).
            \return Number of elements */
        int32_t size() const;

        /** Check emptiness (SCARD).
            \return true iff this set is empty */
        bool empty() const;

        /** Sort operation (SORT).
            Creates s SortOperation to sort the elements of this list.
            Sort order, keys, and return values are configured on the SortOperation. */
        SortOperation sort() const;
    };

} } }

#endif
