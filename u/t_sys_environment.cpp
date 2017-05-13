/**
  *  \file u/t_sys_environment.cpp
  *  \brief Test for afl::sys::Environment
  */

#include "afl/sys/environment.hpp"

#include "u/t_sys.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/directory.hpp"    // required to properly destroy the result of openDirectory()

namespace {
    // Test argv. Since we cannot give it a real argv, give it a fake one,
    // and just test that we get something valid back.
    const char*const argv[] = { "prog", "arg1", "arg2", 0 };
}

/** Test getCommandLine. */
void
TestSysEnvironment::testArgs()
{
    // Command line.
    // Iterating must work without crash.
    // Since we may or may not be iterating over the fake argv above, we cannot know what we get.
    afl::base::Ref<afl::base::Enumerator<String_t> > cmdl = afl::sys::Environment::getInstance(argv).getCommandLine();

    TS_ASSERT(&cmdl.get() != 0);
    String_t tmp;
    while (cmdl->getNextElement(tmp)) {
        // nix
    }

    // Invocation name. Must not be empty.
    TS_ASSERT(afl::sys::Environment::getInstance(argv).getInvocationName() != "");
}

/** Test getEnvironmentVariable. */
void
TestSysEnvironment::testEnv()
{
    // Get some variables. One of them must be set.
    // (When this is ported to a new system, just add one that is always set.)
    String_t v1 = afl::sys::Environment::getInstance(argv).getEnvironmentVariable("PATH");
    String_t v2 = afl::sys::Environment::getInstance(argv).getEnvironmentVariable("SHELL");
    String_t v3 = afl::sys::Environment::getInstance(argv).getEnvironmentVariable("COMSPEC");
    TS_ASSERT(!v1.empty() || !v2.empty() || !v3.empty());

    // Verify sanitation against nulls
    TS_ASSERT_EQUALS(afl::sys::Environment::getInstance(argv).getEnvironmentVariable(String_t("PATH\0Q", 6)), "");
    TS_ASSERT_EQUALS(afl::sys::Environment::getInstance(argv).getEnvironmentVariable(String_t("SHELL\0Q", 7)), "");
    TS_ASSERT_EQUALS(afl::sys::Environment::getInstance(argv).getEnvironmentVariable(String_t("COMSPEC\0Q", 9)), "");
}

/** Test getSettingsDirectoryName. */
void
TestSysEnvironment::testSettings()
{
    // Get some names. Must all be non-empty, different.
    String_t v1 = afl::sys::Environment::getInstance(argv).getSettingsDirectoryName("My App");
    String_t v2 = afl::sys::Environment::getInstance(argv).getSettingsDirectoryName("My App 2.0");
    String_t v3 = afl::sys::Environment::getInstance(argv).getSettingsDirectoryName("My");
    TS_ASSERT(!v1.empty());
    TS_ASSERT(!v2.empty());
    TS_ASSERT(!v3.empty());
    TS_ASSERT_DIFFERS(v1, v2);
    TS_ASSERT_DIFFERS(v2, v3);
    TS_ASSERT_DIFFERS(v1, v3);
}

/** Interface test. */
void
TestSysEnvironment::testInterface()
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
        virtual afl::base::Ref<afl::io::TextWriter> attachTextWriter(Channel /*ch*/)
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::io::TextReader> attachTextReader(Channel /*ch*/)
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::io::Stream> attachStream(Channel /*ch*/)
            { throw std::runtime_error("no ref"); }
    };
    Tester t;

    // Also test the attach...NT cases
    TS_ASSERT(t.attachTextWriterNT(afl::sys::Environment::Output).get() == 0);
    TS_ASSERT(t.attachTextReaderNT(afl::sys::Environment::Input).get() == 0);
    TS_ASSERT(t.attachStreamNT(afl::sys::Environment::Output).get() == 0);
}

/** Test getInstallationDirectoryName(). */
void
TestSysEnvironment::testInstallationDirectory()
{
    // This test is a little smelly because it uses a faked argv but PosixEnvironment::getInstallationDirectoryName evaluates it.
    // However, even in the case of a faked argv, it should return a sensible result.
    String_t dir = afl::sys::Environment::getInstance(argv).getInstallationDirectoryName();
    TS_ASSERT(!dir.empty());

    // Must be accessible as a directory
    TS_ASSERT_THROWS_NOTHING(afl::io::FileSystem::getInstance().openDirectory(dir));
}

