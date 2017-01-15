/**
  *  \file u/mock/commandhandlermock.hpp
  *  \class CommandHandlerMock
  */
#ifndef AFL_U_MOCK_COMMANDHANDLERMOCK_HPP
#define AFL_U_MOCK_COMMANDHANDLERMOCK_HPP

#include <list>
#include <cxxtest/TestSuite.h>
#include "afl/net/commandhandler.hpp"
#include "afl/container/ptrqueue.hpp"

/** Generic CommandHandler mock.
    Allows the user to configure a sequence of expected parameters which it verifies against the received parameters;
    returns a sequence of results provided by the user.

    Typical use:
    - call addParameterList(), addNewResult() in pairs to define the sequence
    - call an operation that performs call() or callVoid() operations and thereby consumes the sequences

    Note that callVoid() will consume results, reflecting the fact that using callVoid() instead of call() is an optimisation
    to the discretion of a user. */
class CommandHandlerMock : public afl::net::CommandHandler {
 public:
    CommandHandlerMock();

    // CommandHandler:
    virtual Value_t* call(const Segment_t& command);
    virtual void callVoid(const Segment_t& command);

    // CommandHandlerMock:
    /** Add parameter.
        \param list Expected parameters. Must be listed in stringified form, separated by ", ".
        For example, a parameter segment consisting of a string "GET" and an integer 4 will be represented as "GET, 4". */
    void addParameterList(const String_t& list);

    /** Add new result.
        \param expected result, newly allocated */
    void addNewResult(Value_t* value);

 private:
    std::list<String_t> m_parameters;
    afl::container::PtrQueue<Value_t> m_results;
};


#endif
