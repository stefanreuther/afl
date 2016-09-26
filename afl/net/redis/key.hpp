/**
  *  \file afl/net/redis/key.hpp
  *  \brief Class afl::net::redis::Key
  */
#ifndef AFL_AFL_NET_REDIS_KEY_HPP
#define AFL_AFL_NET_REDIS_KEY_HPP

#include "afl/string/string.hpp"
#include "afl/net/commandhandler.hpp"

namespace afl { namespace net { namespace redis {

    /** redis database key.
        Objects of this type describe one key in the redis database.
        Derived classes allow actual operations on the keys.

        The Key object just describes the key; the key need not exist yet.
        In fact, operations to query existence or create the key are member of the Key instance.
        The idea is that instead of passing around a string with the key name, pass around a Key.

        All keys are created referring to a CommandHandler that abstracts the network connection to the redis server.
        When multiple keys interact, as in IntegerListKey::transferElement(),
        they must ultimately refer to the same CommandHandler.

        \todo for some operations, we can actually implement cross-server operations as sort-of client-side sharding. */
    class Key {
     public:
        /** Key type. */
        enum Type {
            None,               ///< No type, i.e.\ key does not exist.
            String,             ///< String. Use StringKey, IntegerKey.
            List,               ///< List. Use ListKey, IntegerListKey, StringListKey.
            Set,                ///< Set. Use SetKey, IntegerSetKey, StringSetKey.
            ZSet,               ///< ZSet. Use ZSetKey.
            Hash,               ///< Hash. Use HashKey.
            Unknown             ///< Unknown. Used when this key has a type we don't recognize.
        };

        /** Constructor.
            \param ch CommandHandler to work on
            \param name Key name */
        Key(CommandHandler& ch, const String_t& name);

        /** Destructor. */
        ~Key();

        /** Check for existence (EXISTS).
            \retval true Key exists
            \retval false Key does not exist */
        bool exists() const;

        /** Remove this key from the database (DEL).
            Makes sure that the key does not exist.
            It is not an error if the key does not exist yet. */
        void remove();

        /** Rename this key (RENAME).
            If a key %newName already exists, it will be overwritten.
            This operation will also change the Key to refer to the new name.
            \param newName new name */
        void renameTo(String_t newName);

        /** Rename this key to non-existant name (RENAMENX).
            If a key %newName already exists, the rename will not be executed.
            If the operation succeeds, this Key will refer to the new name.
            \param newName new name
            \retval true rename successful
            \retval false target already exists, rename not executed */
        bool renameToUnique(String_t newName);

        /** Get type of key (TYPE).
            \return type Type. Nonexistant keys return None. */
        Type getType() const;

        /** Get name of this key.
            \return name */
        const String_t& getName() const;

        /** Get associated CommandHandler.
            \return CommandHandler */
        CommandHandler& getHandler() const;

     private:
        CommandHandler& m_ch;
        String_t m_name;
    };

} } }

#endif
