/**
  *  \file afl/test/assert.hpp
  *  \brief Class afl::test::Assert
  */
#ifndef AFL_AFL_TEST_ASSERT_HPP
#define AFL_AFL_TEST_ASSERT_HPP

#include "afl/base/memory.hpp"
#include "afl/string/format.hpp"
#include "afl/string/string.hpp"
#include "afl/test/makeprintable.hpp"

namespace afl { namespace test {

    /** Assertion helper.
        Assertions are implemented by throwing an afl::except::AssertionFailedException.
        To be able to locate an assertion's source, in informative "location" attribute is stored in the Assert object
        and passed along with the exception.
        Components that generate assertions will take an Assert object so you can pass them that information.
        In addition, assertions take a string to describe the failing assertion.

        Together with afl::except::AssertionFailedException, this class can be used in both production and testing environments:
        - in production environment, for regular assertions (which would normally abort,
          but now can be caught and (for example) logged by a main loop);
        - in testing environment, together with TestRunner. */
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

        /** Check for non-null object.
            \param info Information string to be passed into exception
            \param p Pointer to check
            \return *p
            \throw afl::except::AssertionFailedException if p==0 */
        template<typename T>
        T& checkNonNull(const String_t& info, T* p) const;

        /** Check for null object.
            \param info Information string to be passed into exception
            \param p Pointer to check
            \throw afl::except::AssertionFailedException if p != 0 */
        void checkNull(const String_t& info, const void* p) const;

        /** Check equality condition.
            \tparam A,B Type of data. Must be usable with makePrintable(), thus:
            - all builtin types
            - enums
            - types for which there is a makePrintable() function
            \param info Information string to be passed into exception
            \param a First value
            \param b Second value
            \throw afl::except::AssertionFailedException if a!=b */
        template<typename A, typename B>
        void checkEqual(const String_t& info, const A& a, const B& b) const;

        /** Check inequality condition.
            \tparam A,B Type of data. Must be usable with makePrintable(), thus:
            - all builtin types
            - enums
            - types for which there is a makePrintable() function
            \param info Information string to be passed into exception
            \param a First value
            \param b Second value
            \throw afl::except::AssertionFailedException if a==b */
        template<typename A, typename B>
        void checkDifferent(const String_t& info, const A& a, const B& b) const;

        /** Check less-than condition.
            \tparam A,B Type of data. Must be usable with makePrintable(), thus:
            - all builtin types
            - enums
            - types for which there is a makePrintable() function
            \param info Information string to be passed into exception
            \param a First value
            \param b Second value
            \throw afl::except::AssertionFailedException if a>=b */
        template<typename A, typename B>
        void checkLessThan(const String_t& info, const A& a, const B& b) const;

        /** Check less-or-equal condition.
            \tparam A,B Type of data. Must be usable with makePrintable(), thus:
            - all builtin types
            - enums
            - types for which there is a makePrintable() function
            \param info Information string to be passed into exception
            \param a First value
            \param b Second value
            \throw afl::except::AssertionFailedException if a>b */
        template<typename A, typename B>
        void checkLessEqual(const String_t& info, const A& a, const B& b) const;

        /** Check greater-than condition.
            \tparam A,B Type of data. Must be usable with makePrintable(), thus:
            - all builtin types
            - enums
            - types for which there is a makePrintable() function
            \param info Information string to be passed into exception
            \param a First value
            \param b Second value
            \throw afl::except::AssertionFailedException if a<=b */
        template<typename A, typename B>
        void checkGreaterThan(const String_t& info, const A& a, const B& b) const;

        /** Check greater-or-equal condition.
            \tparam A,B Type of data. Must be usable with makePrintable(), thus:
            - all builtin types
            - enums
            - types for which there is a makePrintable() function
            \param info Information string to be passed into exception
            \param a First value
            \param b Second value
            \throw afl::except::AssertionFailedException if a<b */
        template<typename A, typename B>
        void checkGreaterEqual(const String_t& info, const A& a, const B& b) const;

        /** Check data equality condition.
            \tparam T Type of data. Must be usable with makePrintable(), thus:
            - all builtin types
            - enums
            - types for which there is a makePrintable() function
            \param info Information string to be passed into exception
            \param a First data descriptor
            \param b Second data descriptor
            \throw afl::except::AssertionFailedException if !a.equalContent(b).
                   The exception will report the first differing element and index. */
        template<typename T>
        void checkEqualContent(const String_t& info, afl::base::Memory<const T> a, afl::base::Memory<const T> b) const;

        /** Check that floating-point value is in range.
            \param info Information string to be passed into exception
            \param value Value to check
            \param expected Expected value
            \param range Maximum permitted distance
            \throw afl::except::AssertionFailedException if abs(value-expected) > range */
        void checkNear(const String_t& info, double value, double expected, double range) const;

        /** Check that string contains another string.
            \param info Information string to be passed into exception
            \param haystack Subject value
            \param needle Substring to be located
            \throw afl::except::AssertionFailedException if needle not found in haystack */
        void checkContains(const String_t& info, const String_t& haystack, const String_t& needle) const;

        /** Fail.
            Just throws the exception.
            \param info Information string to be passed into exception
            \throw afl::except::AssertionFailedException */
        void fail(const String_t& info) const;

        /** Make a derived location.
            \param location Extra location information to be added to this one
            \return New Assert object */
        Assert operator()(const char* location) const;

        /** Make a derived location.
            \param location Extra location information to be added to this one
            \return New Assert object */
        Assert operator()(const String_t& location) const;

        /** Get location.
            \return location as passed to constructor */
        const String_t& getLocation() const;

     private:
        String_t m_location;
    };

} }

template<typename T>
T&
afl::test::Assert::checkNonNull(const String_t& info, T* p) const
{
    if (p == 0) {
        fail(afl::string::Format("%s (found null)", info));
    }
    return *p;
}

template<typename A, typename B>
void
afl::test::Assert::checkEqual(const String_t& info, const A& a, const B& b) const
{
    if (a != b) {
        fail(afl::string::Format("%s (found %Q != %Q)", info, makePrintable(a), makePrintable(b)));
    }
}

template<typename A, typename B>
void
afl::test::Assert::checkLessThan(const String_t& info, const A& a, const B& b) const
{
    if (!(a < b)) {
        fail(afl::string::Format("%s (found %Q >= %Q)", info, makePrintable(a), makePrintable(b)));
    }
}

template<typename A, typename B>
void
afl::test::Assert::checkLessEqual(const String_t& info, const A& a, const B& b) const
{
    if (!(a <= b)) {
        fail(afl::string::Format("%s (found %Q > %Q)", info, makePrintable(a), makePrintable(b)));
    }
}

template<typename A, typename B>
void
afl::test::Assert::checkGreaterThan(const String_t& info, const A& a, const B& b) const
{
    if (!(a > b)) {
        fail(afl::string::Format("%s (found %Q <= %Q)", info, makePrintable(a), makePrintable(b)));
    }
}

template<typename A, typename B>
void
afl::test::Assert::checkGreaterEqual(const String_t& info, const A& a, const B& b) const
{
    if (!(a >= b)) {
        fail(afl::string::Format("%s (found %Q < %Q)", info, makePrintable(a), makePrintable(b)));
    }
}

template<typename A, typename B>
void
afl::test::Assert::checkDifferent(const String_t& info, const A& a, const B& b) const
{
    if (a == b) {
        fail(afl::string::Format("%s (found %Q == %Q)", info, makePrintable(a), makePrintable(b)));
    }
}

template<typename T>
void
afl::test::Assert::checkEqualContent(const String_t& info, afl::base::Memory<const T> a, afl::base::Memory<const T> b) const
{
    if (a.size() != b.size()) {
        fail(afl::string::Format("%s (different size: %d != %d)", info, a.size(), b.size()));
    }

    size_t index = 0;
    const T* pa;
    const T* pb;
    while ((pa = a.eat()) != 0 && (pb = b.eat()) != 0) {
        if (*pa != *pb) {
            fail(afl::string::Format("%s (found index %d: %Q != %Q)", info, index, makePrintable(*pa), makePrintable(*pb)));
        }
        ++index;
    }
}

/** Check for thrown exception.
    For now, this cannot be a member function.
    \param a               Assert object
    \param code            Code to execute (statement or declaration, without trailing semicolon)
    \param ExceptionClass  Expected exception
    \throw afl::except::AssertionFailedException if none or wrong exception received */
#define AFL_CHECK_THROWS(a, code, ExceptionClass)                       \
do {                                                                    \
    const ::afl::test::Assert& _a = a;                                  \
    bool _threw = false;                                                \
    try { code; }                                                       \
    catch (ExceptionClass&) {                                           \
        _threw = true;                                                  \
    }                                                                   \
    catch (...) {                                                       \
        _threw = true;                                                  \
        _a.fail(afl::string::Format("expected '%s', but code threw something else", #ExceptionClass)); \
    }                                                                   \
    if (!_threw) {                                                      \
        _a.fail(afl::string::Format("expected exception '%s', but code did not throw", #ExceptionClass)); \
    }                                                                   \
} while (0)

/** Check for no thrown exception.
    For now, this cannot be a member function.
    \param a               Assert object
    \param code            Code to execute (statement or declaration, without trailing semicolon)
    \throw afl::except::AssertionFailedException if code threw an exception */
#define AFL_CHECK_SUCCEEDS(a, code)                                     \
do {                                                                    \
    const ::afl::test::Assert& _a = a;                                  \
    try { code; }                                                       \
    catch (...) {                                                       \
        _a.fail(afl::string::Format("expected success, but code threw an exception")); \
    }                                                                   \
} while (0)

#endif
