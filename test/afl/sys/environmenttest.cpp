/**
  *  \file test/afl/sys/environmenttest.cpp
  *  \brief Test for afl::sys::Environment
  */

#include "afl/sys/environment.hpp"

#include <stdexcept>
#include "afl/io/directory.hpp"    // required to properly destroy the result of openDirectory()
#include "afl/io/filesystem.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    // Test argv. Since we cannot give it a real argv, give it a fake one,
    // and just test that we get something valid back.
    const char*const argv[] = { "prog", "arg1", "arg2", 0 };
}

/** Test getCommandLine. */
AFL_TEST("afl.sys.Environment:getCommandLine", a)
{
    // Command line.
    // Iterating must work without crash.
    // Since we may or may not be iterating over the fake argv above, we cannot know what we get.
    afl::base::Ref<afl::base::Enumerator<String_t> > cmdl = afl::sys::Environment::getInstance(argv).getCommandLine();

    a.checkNonNull("command line iterator", &cmdl.get());
    String_t tmp;
    while (cmdl->getNextElement(tmp)) {
        // nix
    }

    // Invocation name. Must not be empty.
    a.check("getInvocationName", afl::sys::Environment::getInstance(argv).getInvocationName() != "");
}

/** Test getEnvironmentVariable. */
AFL_TEST("afl.sys.Environment:getEnvironmentVariable", a)
{
    // Get some variables. One of them must be set.
    // (When this is ported to a new system, just add one that is always set.)
    String_t v1 = afl::sys::Environment::getInstance(argv).getEnvironmentVariable("PATH");
    String_t v2 = afl::sys::Environment::getInstance(argv).getEnvironmentVariable("SHELL");
    String_t v3 = afl::sys::Environment::getInstance(argv).getEnvironmentVariable("COMSPEC");
    a.check("has minimum environment variable", !v1.empty() || !v2.empty() || !v3.empty());

    // Verify sanitation against nulls
    a.checkEqual("sanitation 1", afl::sys::Environment::getInstance(argv).getEnvironmentVariable(String_t("PATH\0Q", 6)), "");
    a.checkEqual("sanitation 2", afl::sys::Environment::getInstance(argv).getEnvironmentVariable(String_t("SHELL\0Q", 7)), "");
    a.checkEqual("sanitation 3", afl::sys::Environment::getInstance(argv).getEnvironmentVariable(String_t("COMSPEC\0Q", 9)), "");
}

/** Test getSettingsDirectoryName. */
AFL_TEST("afl.sys.Environment:getSettingsDirectoryName", a)
{
    // Get some names. Must all be non-empty, different.
    String_t v1 = afl::sys::Environment::getInstance(argv).getSettingsDirectoryName("My App");
    String_t v2 = afl::sys::Environment::getInstance(argv).getSettingsDirectoryName("My App 2.0");
    String_t v3 = afl::sys::Environment::getInstance(argv).getSettingsDirectoryName("My");
    a.check("have dir 1", !v1.empty());
    a.check("have dir 2", !v2.empty());
    a.check("have dir 3", !v3.empty());
    a.checkDifferent("different dir 1", v1, v2);
    a.checkDifferent("different dir 2", v2, v3);
    a.checkDifferent("different dir 3", v1, v3);
}

/** Test interface (and ...NT functions). */
AFL_TEST("afl.sys.Environment:interface", a)
{
    class Tester : public afl::sys::Environment {
     public:
        virtual afl::base::Ref<CommandLine_t> getCommandLine()
            { throw std::runtime_error("no ref"); }
        virtual String_t getInvocationName()
            { return String_t(); }
        virtual String_t getEnvironmentVariable(const String_t& /*name*/)
            { return String_t(); }
        virtual String_t getSettingsDirectoryName(const String_t& /*appName*/)
            { return String_t(); }
        virtual String_t getInstallationDirectoryName()
            { return String_t(); }
        virtual afl::string::LanguageCode getUserLanguage()
            { return afl::string::LanguageCode(); }
        virtual afl::base::Ref<afl::io::TextWriter> attachTextWriter(Channel /*ch*/)
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::io::TextReader> attachTextReader(Channel /*ch*/)
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::io::Stream> attachStream(Channel /*ch*/)
            { throw std::runtime_error("no ref"); }
    };
    Tester t;

    // Also test the attach...NT cases
    a.checkNull("attachTextWriterNT", t.attachTextWriterNT(afl::sys::Environment::Output).get());
    a.checkNull("attachTextReaderNT", t.attachTextReaderNT(afl::sys::Environment::Input).get());
    a.checkNull("attachStreamNT", t.attachStreamNT(afl::sys::Environment::Output).get());
}

/** Test getInstallationDirectoryName(). */
AFL_TEST("afl.sys.Environment:getInstallationDirectoryName", a)
{
    // This test is a little smelly because it uses a faked argv but PosixEnvironment::getInstallationDirectoryName evaluates it.
    // However, even in the case of a faked argv, it should return a sensible result.
    String_t dir = afl::sys::Environment::getInstance(argv).getInstallationDirectoryName();
    a.check("getInstallationDirectoryName empty", !dir.empty());

    // Must be accessible as a directory
    AFL_CHECK_SUCCEEDS(a("directory can be opened"), afl::io::FileSystem::getInstance().openDirectory(dir));
}
