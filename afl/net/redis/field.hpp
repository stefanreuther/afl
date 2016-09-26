/**
  *  \file afl/net/redis/field.hpp
  *  \brief Class afl::net::redis::Field
  */
#ifndef AFL_AFL_NET_REDIS_FIELD_HPP
#define AFL_AFL_NET_REDIS_FIELD_HPP

#include "afl/string/string.hpp"
#include "afl/net/redis/hashkey.hpp"

namespace afl { namespace net { namespace redis {

    /** Hash field.
        Objects of this type describe a field in a HashKey.
        Derived classes allow actual operations on the fields.

        The Field object just describes the field; the field need not exist yet.
        Like for Key, the idea is to pass around Field objects instead of strings with names. */
    class Field {
     public:
        /** Constructor.
            \param hash Hash key that contains the field.
            \param name Name of the field */
        Field(HashKey& hash, const String_t& name);

        /** Destructor. */
        ~Field();

        /** Check for existence (HEXISTS).
            \retval true Field exists
            \retval false Field does not exist. Note that if the hash does not exist, all fields are reported nonexistant. */
        bool exists() const;

        /** Remove this field from the hash (HDEL).
            Makes sure that the field does not exist.
            It is not an error if the field does not exist yet.
            \retval true Field successfully deleted
            \retval false Field did not exist */
        bool remove();

        /** Get name of this field.
            \return name */
        const String_t& getName() const;

        /** Get associated HashKey.
            \return HashKey */
        const HashKey& getHash() const;

     private:
        HashKey m_hash;
        String_t m_name;
    };

} } }

#endif
