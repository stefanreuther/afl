/**
  *  \file test/afl/string/messagestest.cpp
  *  \brief Test for afl::string::Messages
  */

#include "afl/string/messages.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.string.Messages", a)
{
    // There's not much to test here besides that the header is well-formed.
    a.check("rangeError",                 afl::string::Messages::rangeError().size() != 0);
    a.check("fileNotFound",               afl::string::Messages::fileNotFound().size() != 0);
    a.check("fileExists",                 afl::string::Messages::fileExists().size() != 0);
    a.check("fileTooShort",               afl::string::Messages::fileTooShort().size() != 0);
    a.check("invalidFileName",            afl::string::Messages::invalidFileName().size() != 0);
    a.check("invalidUrl",                 afl::string::Messages::invalidUrl().size() != 0);
    a.check("invalidOperation",           afl::string::Messages::invalidOperation().size() != 0);
    a.check("invalidNumber",              afl::string::Messages::invalidNumber().size() != 0);
    a.check("invalidFileHeader",          afl::string::Messages::invalidFileHeader().size() != 0);
    a.check("invalidCompressedData",      afl::string::Messages::invalidCompressedData().size() != 0);
    a.check("cannotAccessFiles",          afl::string::Messages::cannotAccessFiles().size() != 0);
    a.check("cannotAccessDirectories",    afl::string::Messages::cannotAccessDirectories().size() != 0);
    a.check("cannotWrite",                afl::string::Messages::cannotWrite().size() != 0);
    a.check("networkNameNotFound",        afl::string::Messages::networkNameNotFound().size() != 0);
    a.check("networkError",               afl::string::Messages::networkError().size() != 0);
    a.check("networkConnectionLost",      afl::string::Messages::networkConnectionLost().size() != 0);
    a.check("syntaxError",                afl::string::Messages::syntaxError().size() != 0);
    a.check("unknownError",               afl::string::Messages::unknownError().size() != 0);
    a.check("operationCancelled",         afl::string::Messages::operationCancelled().size() != 0);
    a.check("unsupportedFeature",         afl::string::Messages::unsupportedFeature().size() != 0);
    a.check("unsupportedCompressionMode", afl::string::Messages::unsupportedCompressionMode().size() != 0);
    a.check("unexpectedError",            afl::string::Messages::unexpectedError().size() != 0);
    a.check("rootDirTitle",               afl::string::Messages::rootDirTitle().size() != 0);
    a.check("myFilesLinkTitle",           afl::string::Messages::myFilesLinkTitle().size() != 0);
    a.check("desktopLinkTitle",           afl::string::Messages::desktopLinkTitle().size() != 0);
    a.check("rootLinkTitle",              afl::string::Messages::rootLinkTitle().size() != 0);
    a.check("homeLinkTitle",              afl::string::Messages::homeLinkTitle().size() != 0);
}
