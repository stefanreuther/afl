/**
  *  \file afl/net/redis/stringsetoperation.hpp
  *  \brief Class afl::net::redis::StringSetOperation
  */
#ifndef AFL_AFL_NET_REDIS_STRINGSETOPERATION_HPP
#define AFL_AFL_NET_REDIS_STRINGSETOPERATION_HPP

#include "afl/net/commandhandler.hpp"
#include "afl/data/stringlist.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace net { namespace redis {

    class StringSetKey;

    /** Set operation on string set.
        Generates a command to combine multiple sets.
        You typically use StringSetKey::merge, StringSetKey::intersect, or StringSetKey::remove to create an StringSetOperation.
        On that instance, use andAlso() to add more sets if needed.
        Finally, call one of getAll() or storeTo() to obtain the result. */
    class StringSetOperation {
     public:
        /** Constructor.
            Creates a set operation.
            You typically use StringSetKey::merge, StringSetKey::intersect, or StringSetKey::remove to create an StringSetOperation.
            \param op Operation (command name)
            \param a First set
            \param b Second set */
        StringSetOperation(const char op[], const StringSetKey& a, const StringSetKey& b);

        /** Add additional set.
            \param x Additional set
            \return *this */
        StringSetOperation& andAlso(StringSetKey x);

        /** Get result of set operation.
            Call at most once.
            \param list [out] Result elements will be appended here */
        void getAll(afl::data::StringList_t& list);

        /** Store result of operation into new set.
            Call at most once.
            \param key Key that receives the result. */
        void storeTo(StringSetKey key);

     private:
        CommandHandler& m_ch;
        afl::base::Ptr<afl::data::Segment> m_request;
    };

} } }

#endif
