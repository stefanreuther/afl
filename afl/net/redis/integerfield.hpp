/**
  *  \file afl/net/redis/integerfield.hpp
  *  \brief Class afl::net::redis::IntegerField
  */
#ifndef AFL_AFL_NET_REDIS_INTEGERFIELD_HPP
#define AFL_AFL_NET_REDIS_INTEGERFIELD_HPP

#include "afl/net/redis/field.hpp"

namespace afl { namespace net { namespace redis {

    /** Integer field.
        Represents a field of an hash with an integral value.

        Note that although redis can handle arbitrarily large numbers, this class is limited to 32-bit signed integers.
        Values outside this range will produce a InvalidDataException when queried. */
    class IntegerField : public Field {
     public:
        /** Constructor.
            \param hash Hash key that contains the field.
            \param name Name of the field */
        IntegerField(HashKey& hash, const String_t& name);

        /** Increment by one (HINCRBY).
            Atomically increment the current value by one and returns the new value.
            \return new value */
        int32_t operator++();

        /** Increment (HINCRBY).
            Atomically increment the current value by the given value and returns the new value.
            \param val Value to add
            \return new value */
        int32_t operator+=(int32_t incr);

        /** Increment by one (HINCRBY).
            Atomically decrement the current value by one and returns the new value.
            \return new value */
        int32_t operator--();

        /** Decrement (HINCRBY).
            Atomically decrement the current value by the given value and returns the new value.
            \param val Value to subtract
            \return new value */
        int32_t operator-=(int32_t incr);

        /** Set value (HSET).
            \param newValue new value
            \retval false field already existed and was updated
            \retval true field did not yet exist and was created */
        bool set(int32_t value);

        /** Get current value (HGET).
            \return value */
        int32_t get() const;
    };

} } }

#endif
