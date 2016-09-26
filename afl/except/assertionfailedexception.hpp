/**
  *  \file assertionfailedexception.hpp
  *  \brief Class afl::except::AssertionFailedException
  */
#ifndef ASSERTIONFAILEDEXCEPTION_HPP_INCLUDED
#define ASSERTIONFAILEDEXCEPTION_HPP_INCLUDED

#include <stdexcept>
#include "afl/string/string.hpp"

namespace afl { namespace except {

    /** Assertion failure.
        This exception is used as a mild replacement for hard assertion failures.
        Unlike an assert(), it allows you to catch it.
        Like an assert(), it documents that this feature is a program misbehaviour, not an environmental or user error.

        Unlike assert(), it allows (and requires) you to determine what and how to disclose about the failure.

        Unlike assert(), it has no means of being configured out with a preprocessor macro.
        This is normally a bad idea anyway because it can be a source of ODR violations when templates and/or inline functions are participating. */
    class AssertionFailedException : public std::runtime_error {
     public:
        /** Constructor.
            \param text Failing assertion. Could be a stringified expression or a verbal description.
            \param location Location. Could be a function or file name, or any other description. Can be empty. */
        explicit AssertionFailedException(String_t text, String_t location);
    };

    /** Check assertion.
        If the test result is false, throws an AssertionFailedException.
        \param value Test result
        \param text Failing assertion. Could be a stringified expression or a verbal description.
        \param location Location. Could be a function or file name, or any other description. Can be empty. */
    void checkAssertion(bool value, String_t text, String_t location);
    void checkAssertion(bool value, const char* text, const char* location);
    void checkAssertion(bool value, String_t text);
    void checkAssertion(bool value, const char* text);

} }

#endif
