/**
  *  \file u/t_sys_internalenvironment.cpp
  *  \brief Test for afl::sys::InternalEnvironment
  */

#include "afl/sys/internalenvironment.hpp"

#include "t_sys.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/internalstream.hpp"

using afl::except::FileProblemException;
using afl::sys::Environment;

/** Test behaviour of empty (uninitialized) InternalEnvironment. */
void
TestSysInternalEnvironment::testEmpty()
{
    afl::sys::InternalEnvironment testee;

    // getCommandLine
    String_t tmp;
    TS_ASSERT(!testee.getCommandLine()->getNextElement(tmp));

    // getInvocationName
    TS_ASSERT_EQUALS(testee.getInvocationName(), "");

    // getEnvironmentVariable
    TS_ASSERT_EQUALS(testee.getEnvironmentVariable(""), "");
    TS_ASSERT_EQUALS(testee.getEnvironmentVariable("PATH"), "");

    // getSettingsDirectoryName
    TS_ASSERT_EQUALS(testee.getSettingsDirectoryName("awesome"), "");

    // getInstallationDirectoryName
    TS_ASSERT_EQUALS(testee.getInstallationDirectoryName(), "");

    // getUserLanguage
    TS_ASSERT(!testee.getUserLanguage().isValid());

    // attach
    TS_ASSERT_THROWS(testee.attachTextWriter(Environment::Output), FileProblemException);
    TS_ASSERT_THROWS(testee.attachTextReader(Environment::Input), FileProblemException);
    TS_ASSERT_THROWS(testee.attachStream(Environment::Error), FileProblemException);
}

/** Test behaviour of getCommandLine(). */
void
TestSysInternalEnvironment::testCommandLine()
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    afl::data::StringList_t cmdl;
    cmdl.push_back("-f");
    cmdl.push_back("x.txt");
    testee.setCommandLine(cmdl);

    // Retrieve
    String_t tmp;
    afl::base::Ref<Environment::CommandLine_t> it(testee.getCommandLine());
    TS_ASSERT(it->getNextElement(tmp));
    TS_ASSERT_EQUALS(tmp, "-f");
    TS_ASSERT(it->getNextElement(tmp));
    TS_ASSERT_EQUALS(tmp, "x.txt");
    TS_ASSERT(!it->getNextElement(tmp));

    // Retrieve again
    afl::base::Ref<Environment::CommandLine_t> it2(testee.getCommandLine());
    TS_ASSERT(it2->getNextElement(tmp));
    TS_ASSERT_EQUALS(tmp, "-f");
}

/** Test string functions. */
void
TestSysInternalEnvironment::testStrings()
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    testee.setInvocationName("inv");
    testee.setEnvironmentVariable("PATH", "/test/bin");
    testee.setSettingsDirectoryName("/set/*/data");
    testee.setInstallationDirectoryName("/install");
    testee.setUserLanguage(afl::string::LanguageCode("fr"));

    // Verify
    TS_ASSERT_EQUALS(testee.getInvocationName(), "inv");
    TS_ASSERT_EQUALS(testee.getEnvironmentVariable(""), "");
    TS_ASSERT_EQUALS(testee.getEnvironmentVariable("PATH"), "/test/bin");
    TS_ASSERT_EQUALS(testee.getSettingsDirectoryName("app"), "/set/app/data");
    TS_ASSERT_EQUALS(testee.getInstallationDirectoryName(), "/install");
    TS_ASSERT_EQUALS(testee.getUserLanguage().get(), "fr");
}

/** Test stream function. */
void
TestSysInternalEnvironment::testStream()
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    afl::base::Ref<afl::io::InternalStream> in = *new afl::io::InternalStream();
    testee.setChannelStream(Environment::Output, in.asPtr());

    // Use it
    testee.attachStream(Environment::Output)->fullWrite(afl::string::toBytes("hello"));

    // Verify
    TS_ASSERT_EQUALS(in->getSize(), 5U);
    TS_ASSERT(in->getContent().equalContent(afl::string::toBytes("hello")));
}

/** Test TextWriter function. */
void
TestSysInternalEnvironment::testWriter()
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    afl::base::Ref<afl::io::InternalStream> in = *new afl::io::InternalStream();
    testee.setChannelStream(Environment::Output, in.asPtr());

    // Use it
    testee.attachTextWriter(Environment::Output)->writeLine("hi");

    // Verify
    TS_ASSERT_EQUALS(in->getSize(), 3U);
    TS_ASSERT(in->getContent().equalContent(afl::string::toBytes("hi\n")));
}

/** Test TextReader function. */
void
TestSysInternalEnvironment::testReader()
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    afl::base::Ref<afl::io::ConstMemoryStream> in = *new afl::io::ConstMemoryStream(afl::string::toBytes("one\ntwo\n"));
    testee.setChannelStream(Environment::Input, in.asPtr());

    // Use it
    String_t tmp;
    TS_ASSERT(testee.attachTextReader(Environment::Input)->readLine(tmp));

    // Verify
    TS_ASSERT_EQUALS(tmp, "one");
}
