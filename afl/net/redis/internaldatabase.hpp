/**
  *  \file afl/net/redis/internaldatabase.hpp
  *  \brief Class afl::net::redis::InternalDatabase
  */
#ifndef AFL_AFL_NET_REDIS_INTERNALDATABASE_HPP
#define AFL_AFL_NET_REDIS_INTERNALDATABASE_HPP

#include <set>
#include "afl/net/commandhandler.hpp"
#include "afl/container/ptrmap.hpp"
#include "afl/data/segmentview.hpp"
#include "afl/sys/mutex.hpp"

namespace afl { namespace net { namespace redis {

    /** Internal implementation of redis data store.
        This implements a simple replacement/mock of a real redis datastore.
        Use an InternalDatabase instead of a resp::Client-connected-to-a-redis-instance.

        This class implements all commands required for Key/Subtree and its derived classes to function.
        Known differences to real redis:
        - implements a different layer (CommandHandler, not RESP),
          and thus cannot transport protocol nuances like one-line vs. bulk strings)
        - SORT .. STORE returns an empty array (element count in redis)
        - sort BY and GET accept "*" as hash field names
        - KEYS only allows a single "*" operator (which is what Subtree::getKeyNames generates)
        - does not do multithreading, therefore no blocking primitives such as BLPOP or pub/sub
        - numeric values are int32_t, not int64_t (INCR etc.) or double (sort keys)

        As of 20151025, this implementation is very simple and not optimized for memory or speed efficiency. */
    class InternalDatabase : public CommandHandler {
     public:
        /** Constructor. Make an empty database. */
        InternalDatabase();

        /** Destructor. */
        ~InternalDatabase();

        // CommandHandler methods:
        virtual Value_t* call(const Segment_t& command);
        virtual void callVoid(const Segment_t& command);

     private:
        // Value classes
        class Key;
        class String;
        class Hash;
        class Sortable;
        class List;
        class Set;

        // Utility class
        class Sorter;
        friend class Sorter;

        // Synchronisation
        afl::sys::Mutex m_mutex;

        // Data
        typedef afl::container::PtrMap<String_t, Key> Data_t;
        Data_t m_data;

        /** Execute command.
            Factored out for simplicity.
            \param verb Command verb
            \param v Remaining arguments */
        Value_t* execute(const String_t& verb, afl::data::SegmentView v);

        /** Get key, given a type.
            \param name Key
            \return Pointer to given type if found, null if not found
            throw RemoteErrorException if found with different type */
        template<typename T>
        T* get(const String_t& name) const;

        /** Get key, given a type, create if needed.
            \param name Key
            \return Reference to given type, created if needed
            throw RemoteErrorException if found with different type */
        template<typename T>
        T& getCreate(const String_t& name);

        enum SetOperation {
            Difference,
            Intersection,
            Union
        };

        /** Execute a set operation.
            \param op Operation to perform
            \param out Output is produced here
            \param v List of input parameters (key names); must not be empty, will be consumed */
        void executeSetOperation(SetOperation op, std::set<String_t>& out, afl::data::SegmentView& v);

        /** Execute a sort operation.
            \param key Origin key. May be null.
            \param out Output is produced here
            \param v List of additional parameters; will be consumed */
        void executeSortOperation(Sortable* key, Segment_t& out, afl::data::SegmentView& v);

        /** Get value for sorting.
            \param originValue Origin value
            \param pattern User-specified pattern ("#", "bla*->blub", etc.)
            \param result Result will be placed here
            \retval true Result has been produced
            \retval false Target value not found; \c result unchanged */
        bool getSortValue(const String_t& originValue, String_t pattern, String_t& result);
    };

} } }

#endif
