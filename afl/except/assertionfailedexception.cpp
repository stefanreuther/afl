/**
  *  \file afl/except/assertionfailedexception.cpp
  *  \brief Class afl::except::AssertionFailedException
  */

#include "afl/except/assertionfailedexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/format.hpp"

namespace {
    String_t formatAssertion(const String_t& text,
                             const String_t& location)
    {
        return afl::string::Format(location.empty()
                                   ? afl::string::Messages::assertionFailed().c_str()
                                   : afl::string::Messages::assertionFailedAt().c_str(),
                                   text,
                                   location);
    }
}

afl::except::AssertionFailedException::AssertionFailedException(String_t text,
                                                                String_t location)
    : std::runtime_error(formatAssertion(text, location))
{ }

void
afl::except::checkAssertion(bool value, String_t text, String_t location)
{
    if (!value) {
        throw AssertionFailedException(text, location);
    }
}

void
afl::except::checkAssertion(bool value, const char* text, const char* location)
{
    checkAssertion(value, String_t(text), String_t(location));
}

void
afl::except::checkAssertion(bool value, String_t text)
{
    checkAssertion(value, text, String_t());
}

void
afl::except::checkAssertion(bool value, const char* text)
{
    checkAssertion(value, String_t(text), String_t());
}
