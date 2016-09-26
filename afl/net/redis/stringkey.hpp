/**
  *  \file afl/net/redis/stringkey.hpp
  *  \brief Class afl::net::redis::StringKey
  */
#ifndef AFL_AFL_NET_REDIS_STRINGKEY_HPP
#define AFL_AFL_NET_REDIS_STRINGKEY_HPP

#include "afl/net/redis/key.hpp"

namespace afl { namespace net { namespace redis {

    /** String key.
        Represents a normal (string) key. */
    class StringKey : public Key {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        StringKey(CommandHandler& ch, String_t name);

        /** Append text (APPEND).
            If the key already exists, appends some text.
            If the key does not exist, creates it.
            \param text Text to append */
        void append(const String_t& text);

        /** Get current value (GET).
            \return value */
        String_t get() const;

        /** Get substring (GETRANGE [aka SUBSTR]).
            Strings are interpreted as byte strings, not unicode characters (like in std::string).
            Strings have a maximum size of 512 MiB.
            \param start 0-based position of first byte to extract
            \param length maximum number of bytes to extract
            \return extracted bytes */
        String_t substr(std::size_t start, std::size_t length) const;

        /** Replace with a new value (GETSET).
            Returns the old value.
            \param newValue New value
            \return old value */
        String_t replaceBy(const String_t& newValue);

        /** Set value (SET).
            \param newValue new value */
        void set(const String_t& newValue);

        /** Set value for new key (SETNX).
            This operation fails if this key already exists in the database.
            \param newValue new value
            \retval true Operation succeeded, key has been created
            \retval false Key already existed */
        bool setUnique(const String_t& newValue);

        /** Get length of string (STRLEN).
            \return length of string, in bytes */
        size_t size() const;
    };

} } }

#endif
