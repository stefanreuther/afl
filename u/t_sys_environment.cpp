/**
  *  \file u/t_sys_environment.cpp
  *  \brief Test for afl::sys::Environment
  */

#include "afl/sys/environment.hpp"

#include "u/t_sys.hpp"

namespace {
    // Test argv. Since we cannot give it a real argv, give it a fake one,
    // and just test that we get something valid back.
    const char*const argv[] = { "prog", "arg1", "arg2", 0 };
}

/** Test getCommandLine. */
void
TestSysEnvironment::testArgs()
{
    afl::base::Ptr<afl::base::Enumerator<String_t> > cmdl =
        afl::sys::Environment::getInstance(argv).getCommandLine();

    TS_ASSERT(cmdl.get() != 0);
    String_t tmp;
    while (cmdl->getNextElement(tmp)) {
        // nix
    }
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
