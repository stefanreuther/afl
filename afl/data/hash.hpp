/**
  *  \file afl/data/hash.hpp
  *  \brief Class afl::data::Hash
  */
#ifndef AFL_AFL_DATA_HASH_HPP
#define AFL_AFL_DATA_HASH_HPP

#include "afl/base/ref.hpp"
#include "afl/base/refcounted.hpp"
#include "afl/data/namemap.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/value.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace data {

    /** Hash. Represents a key->value mapping.
        Hash objects must always be heap-allocated,
        and are typically referenced from HashValue instances. */
    class Hash : public afl::base::RefCounted {
     public:
        /** Constructor.
            Makes an empty mapping.
            This is a function to make sure all Hash instances are heap-allocated,
            which is required for safe usage in HashValue.
            \return new instance, never null */
        static afl::base::Ref<Hash> create();

        /** Constructor.
            Makes a populated mapping.
            This is a function to make sure all Hash instances are heap-allocated,
            which is required for safe usage in HashValue.
            \param keys Keys. Will be modified (emptied).
            \param values Values. Will be modified (emptied), Hash takes ownership of contained values.
            \return new instance, never null */
        static afl::base::Ref<Hash> create(NameMap& keys, Segment& values);

        /** Destructor. */
        ~Hash();

        /** Set element by key.
            Ownership for the value remains at caller, even if an exception is thrown.
            The value will be cloned.
            \param key Key to access
            \param value Value to set */
        void set(const String_t& key, Value* value);

        /** Set element by key.
            Ownership for the value will be taken over by the Hash; if an exception is thrown, it is deleted.
            The value will not be cloned.
            \param key Key to access
            \param value Value to set */
        void setNew(const String_t& key, Value* value);

        /** Get element by key.
            \param key Key to access
            \return value (owned by the Hash), or null */
        Value* get(const String_t& key) const;

        /** Get NameMap containing all the keys. */
        const NameMap& getKeys() const;

        /** Get Segment containing all the values. */
        const Segment& getValues() const;

     private:
        /** Constructor.
            Makes an empty mapping.
            This constructor is private, use create() to create a heap-allocated Hash. */
        Hash();

        /** Copy constructor. Not implemented.
            (Hash is implicitly uncopyable because the PtrVector contained in the Segment is uncopyable.) */
        Hash(const Hash& other);

        NameMap m_keys;
        Segment m_values;
    };

} }

#endif
