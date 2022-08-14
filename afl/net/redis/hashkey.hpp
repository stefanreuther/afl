/**
  *  \file afl/net/redis/hashkey.hpp
  *  \brief Class afl::net::redis::HashKey
  */
#ifndef AFL_AFL_NET_REDIS_HASHKEY_HPP
#define AFL_AFL_NET_REDIS_HASHKEY_HPP

#include "afl/data/stringlist.hpp"
#include "afl/net/redis/key.hpp"

namespace afl { namespace net { namespace redis {

    class Field;
    class IntegerField;
    class StringField;

    /** Hash key.
        Represents a hash (name->value map).
        The individual elements are called fields and are accessed using Field instances.
        The Hash class itself allows access to the hash as whole. */
    class HashKey : public Key {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        HashKey(CommandHandler& ch, String_t name);

        /** Get number of fields in hash (HLEN).
            \return Number of fields */
        int32_t size() const;

        /** Get names of all fields in hash (HKEYS).
            \param fieldNames [in/out] Field names will be appended here */
        void getFieldNames(afl::data::StringList_t& fieldNames) const;

        /** Access a field.
            Provides untyped access to a field, to check for basic things like existence.
            \param fieldName name of field
            \return handle to field */
        Field field(const String_t& fieldName);

        /** Access integer field.
            Provides typed access to a field with an integral value.
            \param fieldName name of field
            \return handle to field */
        IntegerField intField(const String_t& fieldName);

        /** Access string field.
            Provides typed access to a field with a string value.
            \param fieldName name of field
            \return handle to field */
        StringField stringField(const String_t& fieldName);

        /** Get all content (HGETALL).
            Produces a list of alternating keys and values in \c result.
            \param result [in/out] Keys and values will be appended here */
        void getAll(afl::data::StringList_t& result) const;

        /** Set multiple fields (HMSET).
            Requires a list of alternating keys and values in \c vec.
            \param vec Key/value list */
        void setAll(const afl::data::StringList_t& vec);
    };

} } }

#endif
