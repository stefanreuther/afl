/**
  *  \file afl/net/redis/subtree.hpp
  *  \brief Class afl::net::redis::Subtree
  */
#ifndef AFL_AFL_NET_REDIS_SUBTREE_HPP
#define AFL_AFL_NET_REDIS_SUBTREE_HPP

#include "afl/net/commandhandler.hpp"
#include "afl/net/redis/hashkey.hpp"
#include "afl/net/redis/integerkey.hpp"
#include "afl/net/redis/integerlistkey.hpp"
#include "afl/net/redis/integersetkey.hpp"
#include "afl/net/redis/stringkey.hpp"
#include "afl/net/redis/stringlistkey.hpp"
#include "afl/net/redis/stringsetkey.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace redis {

    /** Subtree of a redis database.
        This class simplifies dealing with structured keyspaces by providing a common root (prefix).
        It is good practice to derive all key classes (IntegerKey etc.) from a Subtree instance
        to allow easier coexistence of multiple applications or functional areas in one database.
        For example, <tt>Subtree(cc, "x:")</tt> will add "x:" to all key names.
        Thus, <tt>Subtree(cc, "x:").intKey("y")</tt> will access "x:y".
        <tt>Subtree(cc, "")</tt> is the "root" of the database (no prefix).

        A subtree can also be used to derive further subtrees derived with colons.
        <tt>Subtree(cc, "").subtree("a").subtree("b").intKey("c")</tt> will access "a:b:c". */
    class Subtree {
     public:
        /** Constructor.
            \param ch CommandHandler to work on
            \param name Subtree name (=prefix). name="" represents the root of the database. */
        Subtree(CommandHandler& ch, const String_t& name);

        /** Destructor. */
        ~Subtree();

        /** Get names of all subkeys within this subtree (KEYS).
            For example, if this subtree represents "a:", and there are "a:x", "a:y", and "a:z", this function may return "x", "y", and "z".
            \param keyNames [in/out] Key names will be appended to this list. Beware that this list may get very large. */
        void getKeyNames(afl::data::StringList_t& keyNames);

        /** Get access to a hash key.
            \param name Name of key
            \return handle to hash key */
        HashKey hashKey(const String_t& name) const;
        // FIXME: DbHash hashKey(const char* name) const;

        /** Get access to a integer key.
            \param name Name of key
            \return handle to integer key */
        IntegerKey intKey(const String_t& name) const;
        // FIXME: DbInt intKey(const char* name) const;

        /** Get access to a string key.
            \param name Name of key
            \return handle to string key */
        StringKey stringKey(const String_t& name) const;
        // FIXME: DbString stringKey(const char* name) const;

        /** Get access to an integer list key. */
        IntegerListKey intListKey(const String_t& name) const;
        // FIXME: DbIntList intListKey(const char* name) const;

        /** Get access to a string list key. */
        StringListKey stringListKey(const String_t& name) const;
        // FIXME: DbStringList stringListKey(const char* name) const;


        /** Get access to integer set key.
            \param name Name of key
            \return handle to integer set key */
        IntegerSetKey intSetKey(const String_t& name) const;
        // FIXME: DbIntSet intSetKey(const char* name) const;

        /** Get access to string set key.
            \param name Name of key
            \return handle to string set key */
        StringSetKey stringSetKey(const String_t& name) const;
        // FIXME: DbStringSet stringSetKey(const char* name) const;

        /** Get access to untyped key.
            \param name Name of key
            \return handle to key */
        Key key(const String_t& name) const;

        // DbZSet       zsetKey(const string_t& name) const;
        // DbZSet       zsetKey(const char* name) const;


        /** Get access to a subtree.
            \param name Name of subtree
            \return handle to subtree */
        Subtree subtree(const String_t& name) const;
        // FIXME: DbSubtree subtree(const char* name) const;

        /** Get access to a subtree, integer name.
            \param name Number ot use as name of subtree
            \return handle to subtree */
        Subtree subtree(int32_t name) const;

        /** Get name (prefix) of this subtree.
            \return prefix */
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
