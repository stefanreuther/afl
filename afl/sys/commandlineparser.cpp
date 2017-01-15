/**
  *  \file afl/sys/commandlineparser.cpp
  *  \brief Interface afl::sys::CommandLineParser
  */

#include "afl/sys/commandlineparser.hpp"
#include "afl/string/format.hpp"
#include "afl/string/messages.hpp"
#include "afl/except/commandlineexception.hpp"

// Get a required parameter.
String_t
afl::sys::CommandLineParser::getRequiredParameter(const String_t& text)
{
    // Save flags because getParameter might clobber them
    Flags_t savedFlags = getFlags();

    // Fetch parameter
    String_t result;
    if (getParameter(result)) {
        // ok
        return result;
    } else {
        // parameter is missing
        String_t name = "-";
        if (savedFlags.contains(IsLongOption)) {
            name += "-";
        }
        name += text;
        throw afl::except::CommandLineException(afl::string::Format(afl::string::Messages::missingCommandLineParameter().c_str(), name));
    }
}
