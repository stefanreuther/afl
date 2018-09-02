/**
  *  \file afl/net/redis/integerkey.hpp
  *  \brief Class afl::net::redis::IntegerKey
  */
#ifndef AFL_AFL_NET_REDIS_INTEGERKEY_HPP
#define AFL_AFL_NET_REDIS_INTEGERKEY_HPP

#include "afl/net/redis/key.hpp"
#include "afl/base/optional.hpp"

namespace afl { namespace net { namespace redis {

    /** Integer key.
        Represents a normal (string) key containing an integral numeric value.

        Note that although redis can handle arbitrarily large numbers, this class is limited to 32-bit signed integers.
        Values outside this range will produce a InvalidDataException when queried. */
    class IntegerKey : public Key {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        IntegerKey(CommandHandler& ch, String_t name);

        /** Decrement by one (DECR).
            Atomically reduces the current value by one and returns the new value.
            \return new value */
        int32_t operator--();

        /** Decrement (DECRBY).
            Atomically reduces the current value by the given value and returns the new value.
            \param val Value to subtract
            \return new value */
        int32_t operator-=(int32_t val);

        /** Get current value (GET).
            \return value */
        int32_t get() const;

        /** Get current value if it exists (GET).
            \return value (Nothing if it is not set) */
        afl::base::Optional<int32_t> getOptional() const;

        /** Replace with a new value (GETSET).
            Returns the old value.
            \param newValue New value
            \return old value */
        int32_t replaceBy(int32_t newValue);

        /** Increment by one (INCR).
            Atomically increment the current value by one and returns the new value.
            \return new value */
        int32_t operator++();

        /** Increment (INCRBY).
            Atomically increment the current value by the given value and returns the new value.
            \param val Value to add
            \return new value */
        int32_t operator+=(int32_t val);

        /** Set value (SET).
            \param newValue new value */
        void set(int32_t newValue);

        /** Set value for new key (SETNX).
            This operation fails if this key already exists in the database.
            \param newValue new value
            \retval true Operation succeeded, key has been created
            \retval false Key already existed */
        bool setUnique(int32_t newValue);
    };

} } }

#endif
