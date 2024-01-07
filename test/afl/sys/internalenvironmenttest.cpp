/**
  *  \file test/afl/sys/internalenvironmenttest.cpp
  *  \brief Test for afl::sys::InternalEnvironment
  */

#include "afl/sys/internalenvironment.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/test/testrunner.hpp"

using afl::except::FileProblemException;
using afl::sys::Environment;

/** Test behaviour of empty (uninitialized) InternalEnvironment. */
AFL_TEST("afl.sys.InternalEnvironment:empty", a)
{
    afl::sys::InternalEnvironment testee;

    // getCommandLine
    String_t tmp;
    a.check("01. getCommandLine", !testee.getCommandLine()->getNextElement(tmp));

    // getInvocationName
    a.checkEqual("02. getInvocationName", testee.getInvocationName(), "");

    // getEnvironmentVariable
    a.checkEqual("03. getEnvironmentVariable", testee.getEnvironmentVariable(""), "");
    a.checkEqual("04. getEnvironmentVariable", testee.getEnvironmentVariable("PATH"), "");

    // getSettingsDirectoryName
    a.checkEqual("05. getSettingsDirectoryName", testee.getSettingsDirectoryName("awesome"), "");

    // getInstallationDirectoryName
    a.checkEqual("06. getInstallationDirectoryName", testee.getInstallationDirectoryName(), "");

    // getUserLanguage
    a.check("07. getUserLanguage", !testee.getUserLanguage().isValid());

    // attach
    AFL_CHECK_THROWS(a("08. Output"), testee.attachTextWriter(Environment::Output), FileProblemException);
    AFL_CHECK_THROWS(a("09. Input"), testee.attachTextReader(Environment::Input), FileProblemException);
    AFL_CHECK_THROWS(a("10. Error"), testee.attachStream(Environment::Error), FileProblemException);
}

/** Test behaviour of getCommandLine(). */
AFL_TEST("afl.sys.InternalEnvironment:getCommandLine", a)
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
    a.check("01. has element", it->getNextElement(tmp));
    a.checkEqual("02. element value", tmp, "-f");
    a.check("03. has element", it->getNextElement(tmp));
    a.checkEqual("04. element value", tmp, "x.txt");
    a.check("05. no next element", !it->getNextElement(tmp));

    // Retrieve again
    afl::base::Ref<Environment::CommandLine_t> it2(testee.getCommandLine());
    a.check("11. has element", it2->getNextElement(tmp));
    a.checkEqual("21. element value", tmp, "-f");
}

/** Test string functions. */
AFL_TEST("afl.sys.InternalEnvironment:string-functions", a)
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    testee.setInvocationName("inv");
    testee.setEnvironmentVariable("PATH", "/test/bin");
    testee.setSettingsDirectoryName("/set/*/data");
    testee.setInstallationDirectoryName("/install");
    testee.setUserLanguage(afl::string::LanguageCode("fr"));

    // Verify
    a.checkEqual("01. getInvocationName",            testee.getInvocationName(), "inv");
    a.checkEqual("02. getEnvironmentVariable",       testee.getEnvironmentVariable(""), "");
    a.checkEqual("03. getEnvironmentVariable",       testee.getEnvironmentVariable("PATH"), "/test/bin");
    a.checkEqual("04. getSettingsDirectoryName",     testee.getSettingsDirectoryName("app"), "/set/app/data");
    a.checkEqual("05. getInstallationDirectoryName", testee.getInstallationDirectoryName(), "/install");
    a.checkEqual("06. getUserLanguage",              testee.getUserLanguage().get(), "fr");
}

/** Test stream function. */
AFL_TEST("afl.sys.InternalEnvironment:attachStream", a)
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    afl::base::Ref<afl::io::InternalStream> in = *new afl::io::InternalStream();
    testee.setChannelStream(Environment::Output, in.asPtr());

    // Use it
    testee.attachStream(Environment::Output)->fullWrite(afl::string::toBytes("hello"));

    // Verify
    a.checkEqual("size", in->getSize(), 5U);
    a.check("content", in->getContent().equalContent(afl::string::toBytes("hello")));
}

/** Test TextWriter function. */
AFL_TEST("afl.sys.InternalEnvironment:attachTextWriter", a)
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    afl::base::Ref<afl::io::InternalStream> in = *new afl::io::InternalStream();
    testee.setChannelStream(Environment::Output, in.asPtr());

    // Use it
    testee.attachTextWriter(Environment::Output)->writeLine("hi");

    // Verify
    a.checkEqual("size", in->getSize(), 3U);
    a.check("content", in->getContent().equalContent(afl::string::toBytes("hi\n")));
}

/** Test TextReader function. */
AFL_TEST("afl.sys.InternalEnvironment:attachTextReader", a)
{
    // Prepare
    afl::sys::InternalEnvironment testee;
    afl::base::Ref<afl::io::ConstMemoryStream> in = *new afl::io::ConstMemoryStream(afl::string::toBytes("one\ntwo\n"));
    testee.setChannelStream(Environment::Input, in.asPtr());

    // Use it
    String_t tmp;
    a.check("read line", testee.attachTextReader(Environment::Input)->readLine(tmp));

    // Verify
    a.checkEqual("result", tmp, "one");
}
