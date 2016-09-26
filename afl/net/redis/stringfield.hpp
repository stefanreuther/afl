/**
  *  \file afl/net/redis/stringfield.hpp
  *  \brief Class afl::net::redis::StringField
  */
#ifndef AFL_AFL_NET_REDIS_STRINGFIELD_HPP
#define AFL_AFL_NET_REDIS_STRINGFIELD_HPP

#include "afl/net/redis/field.hpp"

namespace afl { namespace net { namespace redis {

    /** String field.
        Represents a field of an hash. */
    class StringField : public Field {
     public:
        /** Constructor.
            \param hash Hash key that contains the field.
            \param name Name of the field */
        StringField(HashKey& hash, const String_t& name);

        /** Set value (HSET).
            \param newValue new value
            \retval false field already existed and was updated
            \retval true field did not yet exist and was created */
        bool set(String_t value);

        /** Get current value (HGET).
            \return value */
        String_t get() const;
    };

} } }


#endif
