/**
  *  \file afl/test/testrunner.cpp
  *  \brief Class afl::test::TestRunner
  */

#include <cassert>
#include <cstdio>
#include <set>
#include <cstring>
#include "afl/test/testrunner.hpp"
#include "afl/except/assertionfailedexception.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/sys/time.hpp"

namespace {

    const char*const BULLET = "\xE2\x96\xB6 ";

    String_t colorize(String_t text, const char* color)
    {
        return String_t("\33[") + color + "m" + text + "\33[0m";
    }

    bool isEnabled(const String_t& name, const std::set<String_t>& filter)
    {
        if (filter.empty()) {
            return true;
        } else {
            for (std::set<String_t>::const_iterator it = filter.begin(), end = filter.end(); it != end; ++it) {
                if (name.find(*it) != String_t::npos) {
                    return true;
                }
            }
            return false;
        }
    }

    void showProgress(std::ostream& out, const String_t& text, size_t& progressLength)
    {
        out << "\r" << text;

        size_t newLength = text.size();
        if (newLength < progressLength) {
            size_t eraseLength = progressLength - newLength;
            out << String_t(eraseLength, ' ')
                << String_t(eraseLength, '\b');
        }

        out.flush();
        progressLength = newLength;
    }


    /*
     *  Test case report (structure).
     */
    struct TestCaseReport {
        /** Constructor.
            @param name Name of test case
            @param succeeded true if test succeeded
            @param message Message for failure
            @param timeMS Elapsed time in milliseconds */
        TestCaseReport(String_t name, bool succeeded, String_t message, int timeMS)
            : name(name), succeeded(succeeded), message(message), timeMS(timeMS)
            { }

        String_t name;
        bool succeeded;
        String_t message;
        int timeMS;
    };


    /*
     *  TestReport
     */

    class TestReport {
     public:
        /** Constructor.
            Make empty test report. */
        TestReport()
            : m_testCases()
            { }

        /** Add a test case result.
            @param name Test ca
            @param name Name of test case
            @param succeeded true if test succeeded
            @param message Message for failure
            @param timeMS Elapsed time in milliseconds */
        void addTestCase(String_t name, bool succeeded, String_t message, int timeMS)
            {
                m_testCases.pushBackNew(new TestCaseReport(name, succeeded, message, timeMS));
            }

        /** Check overall success.
            @return true if all tests succeeded */
        bool isSucceeded() const
            {
                for (size_t i = 0, n = m_testCases.size(); i < n; ++i) {
                    if (!m_testCases[i]->succeeded) {
                        return false;
                    }
                }
                return true;
            }

        /** Print textual summary.
            @param out PrintWriter to receive output */
        void printSummary(std::ostream& out, bool verbose) const
            {
                int numCases = 0, numFailures = 0;
                for (size_t i = 0, n = m_testCases.size(); i < n; ++i) {
                    const TestCaseReport& caseReport = *m_testCases[i];
                    if (!caseReport.succeeded) {
                        out << colorize(BULLET + caseReport.name, "1") << ": " << colorize("FAILED", "1;31") << "\n"
                            << caseReport.message << "\n";
                        ++numFailures;
                    } else {
                        if (verbose) {
                            out << caseReport.name << ": " << colorize("ok", "1;32") << "\n";
                        }
                    }
                    ++numCases;
                }

                char buffer[200];
                std::sprintf(buffer, "%s%d test cases, %d failures", BULLET, numCases, numFailures);
                out << colorize(buffer, numFailures != 0 ? "1;31" : "1") << "\n" << std::flush;
            }

     private:
        afl::container::PtrVector<TestCaseReport> m_testCases;
    };
}

afl::test::TestRunner::TestRunner()
    : m_tests()
{ }

void
afl::test::TestRunner::addNewTest(const char* name, Function_t* test)
{
    assert(name != 0);
    assert(test != 0);
    std::auto_ptr<Function_t> p(test);
    m_tests.pushBackNew(new Test(p, name));
}

int
afl::test::TestRunner::run(std::ostream& out, const char*const* argv)
{
    std::set<String_t> filter;
    bool verbose = false;
    bool list = false;
    bool progress = false;
    while (const char* p = *++argv) {
        if (std::strcmp(p, "-v") == 0) {
            verbose = true;
        } else if (std::strcmp(p, "-l") == 0) {
            list = true;
        } else if (std::strcmp(p, "-p") == 0) {
            progress = true;
        } else if (*p == '+') {
            filter.insert(String_t(p+1));
        } else {
            out << "Unrecognized parameter '" << p << "'.\n"
                << "Valid parameters:\n"
                << "  -l      list only\n"
                << "  -p      show progress\n"
                << "  -v      verbose report\n"
                << "  +NAME   only execute tests containing NAME\n";
            return 1;
        }
    }

    if (list) {
        for (size_t i = 0, n = m_tests.size(); i < n; ++i) {
            const Test& t = *m_tests[i];
            if (isEnabled(t.name, filter)) {
                out << t.name << "\n";
            }
        }
        return 0;
    }

    TestReport report;
    size_t progressLength = 0;
    for (size_t i = 0, n = m_tests.size(); i < n; ++i) {
        const Test& t = *m_tests[i];
        if (isEnabled(t.name, filter)) {
            // Start tracking
            bool result = false;
            String_t message = "";
            uint32_t startTime = afl::sys::Time::getTickCounter();
            if (progress) {
                char tmp[50];
                std::sprintf(tmp, "%d/%d ", (int) (i+1), (int) n);
                showProgress(out, tmp + t.name, progressLength);
            }

            // Run the test
            try {
                Assert a(t.name);
                t.func->call(a);
                result = true;
            }
            catch (afl::except::AssertionFailedException& ex) {
                message = "AssertionFailedException: ";
                message += ex.what();
                result = false;
            }
            catch (afl::except::FileProblemException& fpe) {
                message = "FileProblemException: ";
                message += fpe.getFileName();
                message += ": ";
                message += fpe.what();
                result = false;
            }
            catch (std::exception& e) {
                message = "Exception: ";
                message += e.what();
                result = false;
            }
            catch (...) {
                message = "Unknown exception";
                result = false;
            }

            // End tracking
            uint32_t endTime = afl::sys::Time::getTickCounter();
            int elapsedMS = (int)endTime - startTime;
            report.addTestCase(t.name, result, message, elapsedMS);
        }
    }

    if (progressLength != 0) {
        showProgress(out, "", progressLength);
    }

    // If testsuite is still empty, fail it
    if (m_tests.empty()) {
        report.addTestCase("<empty>", false, "test suite is empty", 0);
    }

    // Result
    report.printSummary(out, verbose);
    out.flush();

    return report.isSucceeded() ? 0 : 1;
}

afl::test::TestRunner&
afl::test::TestRunner::getInstance()
{
    static TestRunner theInstance;
    return theInstance;
}
