/**
  *  \file afl/net/redis/integersetoperation.hpp
  *  \brief Class afl::net::redis::IntegerSetOperation
  */
#ifndef AFL_AFL_NET_REDIS_INTEGERSETOPERATION_HPP
#define AFL_AFL_NET_REDIS_INTEGERSETOPERATION_HPP

#include "afl/net/commandhandler.hpp"
#include "afl/data/integerlist.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace net { namespace redis {

    class IntegerSetKey;

    /** Set operation on integer set.
        Generates a command to combine multiple sets.
        You typically use IntegerSetKey::merge, IntegerSetKey::intersect, or IntegerSetKey::remove to create an IntegerSetOperation.
        On that instance, use andAlso() to add more sets if needed.
        Finally, call one of getAll() or storeTo() to obtain the result. */
    class IntegerSetOperation {
     public:
        /** Constructor.
            Creates a set operation.
            You typically use IntegerSetKey::merge, IntegerSetKey::intersect, or IntegerSetKey::remove to create an IntegerSetOperation.
            \param op Operation (command name)
            \param a First set
            \param b Second set */
        IntegerSetOperation(const char op[], const IntegerSetKey& a, const IntegerSetKey& b);

        /** Add additional set.
            \param x Additional set
            \return *this */
        IntegerSetOperation& andAlso(IntegerSetKey x);

        /** Get result of set operation.
            Call at most once.
            \param list [out] Result elements will be appended here */
        void getAll(afl::data::IntegerList_t& list);

        /** Store result of operation into new set.
            Call at most once.
            \param key Key that receives the result. */
        void storeTo(IntegerSetKey key);

     private:
        CommandHandler& m_ch;
        afl::base::Ptr<afl::data::Segment> m_request;
    };

} } }

#endif
