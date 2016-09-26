/**
  *  \file u/t_string_messages.cpp
  *  \brief Test for afl::string::Messages
  */

#include "afl/string/messages.hpp"

#include "u/t_string.hpp"

/** Simple test. */
void
TestStringMessages::testIt()
{
    // There's not much to test here besides that the header is well-formed.
    TS_ASSERT(afl::string::Messages::rangeError().size() != 0);
    TS_ASSERT(afl::string::Messages::fileNotFound().size() != 0);
    TS_ASSERT(afl::string::Messages::fileExists().size() != 0);
    TS_ASSERT(afl::string::Messages::fileTooShort().size() != 0);
    TS_ASSERT(afl::string::Messages::invalidFileName().size() != 0);
    TS_ASSERT(afl::string::Messages::invalidUrl().size() != 0);
    TS_ASSERT(afl::string::Messages::invalidOperation().size() != 0);
    TS_ASSERT(afl::string::Messages::invalidNumber().size() != 0);
    TS_ASSERT(afl::string::Messages::invalidFileHeader().size() != 0);
    TS_ASSERT(afl::string::Messages::invalidCompressedData().size() != 0);
    TS_ASSERT(afl::string::Messages::cannotAccessFiles().size() != 0);
    TS_ASSERT(afl::string::Messages::cannotAccessDirectories().size() != 0);
    TS_ASSERT(afl::string::Messages::cannotWrite().size() != 0);
    TS_ASSERT(afl::string::Messages::networkNameNotFound().size() != 0);
    TS_ASSERT(afl::string::Messages::networkError().size() != 0);
    TS_ASSERT(afl::string::Messages::networkConnectionLost().size() != 0);
    TS_ASSERT(afl::string::Messages::syntaxError().size() != 0);
    TS_ASSERT(afl::string::Messages::unknownError().size() != 0);
    TS_ASSERT(afl::string::Messages::operationCancelled().size() != 0);
    TS_ASSERT(afl::string::Messages::unsupportedFeature().size() != 0);
    TS_ASSERT(afl::string::Messages::rootDirTitle().size() != 0);
    TS_ASSERT(afl::string::Messages::myFilesLinkTitle().size() != 0);
    TS_ASSERT(afl::string::Messages::desktopLinkTitle().size() != 0);
    TS_ASSERT(afl::string::Messages::rootLinkTitle().size() != 0);
    TS_ASSERT(afl::string::Messages::homeLinkTitle().size() != 0);
}
