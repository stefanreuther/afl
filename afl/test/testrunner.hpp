/**
  *  \file afl/test/testrunner.hpp
  *  \brief Class afl::test::TestRunner
  */
#ifndef AFL_AFL_TEST_TESTRUNNER_HPP
#define AFL_AFL_TEST_TESTRUNNER_HPP

#include <memory>
#include <ostream>
#include "afl/base/closure.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/string/string.hpp"
#include "afl/test/assert.hpp"

namespace afl { namespace test {

    /** Test runner.
        Contains a list of testcases and allows executing them.
        Together with class Assert and the AFL_TEST macro, this forms a minimal unit-testing framework.

        Although this class can also be used standalone, it is usually used through its global instance:

        To define a testcase, do
        <code>
          AFL_TEST("name", a) {
            // code
          }
        </code>
        This will internally call <tt>TestRunner::addNewTest()</tt> on the global instance.

        Provide a main() function that calls <tt>TestRunner::run(std::cout, argv)</tt> on the global instance.

        This TestRunner just collects a big list of testcases, with no structure into testsuites etc.
        This also means you need not have any bureaucracy to create tests beyond the AFL_TEST macro.
        For setup()/tearDown(), use C++ RAII features (local variables with destructor). */
    class TestRunner {
     public:
        /** Type for a testcase function. */
        typedef afl::base::Closure<void(const Assert&)> Function_t;

        /** Constructor. */
        TestRunner();

        /** Add a new test.
            @param name  Test name
            @param test  Test. Must not be null. TestRunner takes ownership. */
        void addNewTest(const char* name, Function_t* test);

        /** Run all tests.
            @param out   Output will be generated here
            @param argv  Parameters received from command line
            @return exit code to return to operating system */
        int run(std::ostream& out, const char*const* argv);

        /** Get global instance.
            @return global instance */
        static TestRunner& getInstance();

        /** Helper to implement calling addNewTest() at construction time. */
        struct Registrar {
            Registrar(TestRunner& inst, const char* name, Function_t* test)
                { inst.addNewTest(name, test); }
        };

     private:
        struct Test {
            std::auto_ptr<Function_t> func;
            String_t name;

            Test(std::auto_ptr<Function_t> func, const char* name)
                : func(func), name(name)
                { }
        };

        /** List of all registered tests. */
        afl::container::PtrVector<Test> m_tests;
    };

} }

/** Define a testcase.
    Use as
    <code>
      AFL_TEST("name", a) {
        // code
      }
    </code>

    The name "name" should be the unique name of the testcase.
    The name "a" is provded as an Assert object.
    It can be used within the code to execute actual testing.

    In case of a failure, the info string given in the assertion and the name given to AFL_TEST
    will be shown to identify the failing assertion.
    There is no builtin magic to generate a backtrace.

    Note that each AFL_TEST invocation needs to be on a line of its own
    because we use __LINE__ to generate per-file unique names. */
#define AFL_TEST(name, arg) AFL_TEST_LINE(name, arg, __LINE__)
#define AFL_TEST_CAT(a, b) a ## b
#define AFL_TEST_LINE(name, arg, line) \
    static void AFL_TEST_CAT(afl_test_func_, line)(const ::afl::test::Assert&); \
    static ::afl::test::TestRunner::Registrar AFL_TEST_CAT(afl_test_reg_, line)(::afl::test::TestRunner::getInstance(), name, ::afl::test::TestRunner::Function_t::makeStatic(&AFL_TEST_CAT(afl_test_func_, line))); \
    static void AFL_TEST_CAT(afl_test_func_, line)(const ::afl::test::Assert& arg)

/** Define a testcase without asserter argument.
    Use as
    <code>
      AFL_TEST_NOARG("name") {
        // code
      }
    </code>

    The name "name" should be the unique name of the testcase.
    The test does not receive an Assert object as a parameter.
    Use for tests that detect errors in other ways, e.g. pure "check-to-compile" tests,
    static_assert, or exceptions thrown. */
#define AFL_TEST_NOARG(name) AFL_TEST_NOARG_LINE(name, __LINE__)
#define AFL_TEST_NOARG_LINE(name, line)                            \
    static void AFL_TEST_CAT(afl_test_func_, line)(); \
    static ::afl::test::TestRunner::Registrar AFL_TEST_CAT(afl_test_reg_, line)(::afl::test::TestRunner::getInstance(), name, ::afl::test::TestRunner::Function_t::makeStatic(&AFL_TEST_CAT(afl_test_func_, line))); \
    static void AFL_TEST_CAT(afl_test_func_, line)()

#endif
