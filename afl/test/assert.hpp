/**
  *  \file afl/test/assert.hpp
  *  \brief Class afl::test::Assert
  */
#ifndef AFL_AFL_TEST_ASSERT_HPP
#define AFL_AFL_TEST_ASSERT_HPP

#include "afl/string/string.hpp"
#include "afl/base/memory.hpp"
#include "afl/string/format.hpp"

namespace afl { namespace test {

    /** Assertion helper.
        Assertions are implemented by throwing an afl::except::AssertionFailedException.
        To be able to locate an assertion's source, in informative "location" attribute is stored in the Assert object
        and passed along with the exception.
        Components that generate assertions will take an Assert object so you can pass them that information.
        In addition, assertions take a string to describe the failing assertion. */
    class Assert {
     public:
        /** Constructor.
            \param location Location information to be passed into exceptions. */
        Assert(const String_t& location);

        /** Constructor.
            \param location Location information to be passed into exceptions. */
        Assert(const char* location);

        /** Check boolean condition.
            \param info Information string to be passed into exception
            \param condition Condition to check
            \throw afl::except::AssertionFailedException if condition==false */
        void check(const String_t& info, bool condition) const;

        /** Check equality condition.
            \tparam A,B Type of data. Must be usable with afl::string::Format.
            \param info Information string to be passed into exception
            \param a First value
            \param b Second value
            \throw afl::except::AssertionFailedException if a!=b */
        template<typename A, typename B>
        void checkEqual(const String_t& info, const A& a, const B& b) const;

        /** Check data equality condition.
            \tparam T Type of data. Must be usable with afl::string::Format.
            \param info Information string to be passed into exception
            \param a First data descriptor
            \param b Second data descriptor
            \throw afl::except::AssertionFailedException if !a.equalContent(b).
                   The exception will report the first differing element and index. */
        template<typename T>
        void checkEqualContent(const String_t& info, afl::base::Memory<const T> a, afl::base::Memory<const T> b) const;

        /** Fail.
            Just throws the exception.
            \param info Information string to be passed into exception
            \throw afl::except::AssertionFailedException */
        void fail(const String_t& info) const;

     private:
        String_t m_location;
    };

} }

template<typename A, typename B>
void
afl::test::Assert::checkEqual(const String_t& info, const A& a, const B& b) const
{
    if (a != b) {
        fail(afl::string::Format("%s (found %Q != %Q)", info, a, b));
    }
}

template<typename T>
void
afl::test::Assert::checkEqualContent(const String_t& info, afl::base::Memory<const T> a, afl::base::Memory<const T> b) const
{
    if (a.size() != b.size()) {
        fail(afl::string::Format("%s (different size: %d != %d)", a.size(), b.size()));
    }

    size_t index = 0;
    const T* pa;
    const T* pb;
    while ((pa = a.eat()) != 0 && (pb = b.eat()) != 0) {
        if (*pa != *pb) {
            fail(afl::string::Format("%s (found index %d: %Q != %Q)", info, index, *pa, *pb));
        }
        ++index;
    }
}

#endif
