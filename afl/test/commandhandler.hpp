/**
  *  \file afl/test/commandhandler.hpp
  */
#ifndef AFL_AFL_TEST_COMMANDHANDLER_HPP
#define AFL_AFL_TEST_COMMANDHANDLER_HPP

#include <list>
#include "afl/net/commandhandler.hpp"
#include "afl/container/ptrqueue.hpp"
#include "afl/test/assert.hpp"

namespace afl { namespace test {

    /** Generic CommandHandler mock.
        Allows the user to configure a sequence of expected parameters which it verifies against the received parameters;
        returns a sequence of results provided by the user.

        Typical use:
        - call expectCall(), provideNewResult() in pairs to define the sequence
        - call an operation that performs call() or callVoid() operations and thereby consumes the sequences

        Note that callVoid() will consume results, reflecting the fact that using callVoid() instead of call() is an optimisation
        to the discretion of a user. */
    class CommandHandler : public afl::net::CommandHandler {
     public:
        CommandHandler(const Assert& a);

        // CommandHandler:
        virtual Value_t* call(const Segment_t& command);
        virtual void callVoid(const Segment_t& command);

        /** Expect a call (call from test).
            \param list Expected parameters. Must be listed in stringified form, separated by ", ".
            For example, a parameter segment consisting of a string "GET" and an integer 4 will be represented as "GET, 4". */
        void expectCall(const String_t& list);

        /** Provide a return value (call from test).
            \param value expected result, newly allocated */
        void provideNewResult(Value_t* value);

        /** Verify that call sequence is complete (call from test).
            \throw afl::except::AssertionFailedException if not all expected calls have been made, all return values have been consumed */
        void checkFinish();

     private:
        Assert m_assert;
        std::list<String_t> m_queue;
        afl::container::PtrQueue<Value_t> m_results;
    };

} }

#endif
