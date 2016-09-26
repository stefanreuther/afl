/**
  *  \file afl/sys/longcommandlineparser.hpp
  */
#ifndef AFL_AFL_SYS_LONGCOMMANDLINEPARSER_HPP
#define AFL_AFL_SYS_LONGCOMMANDLINEPARSER_HPP

#include "afl/sys/commandlineparser.hpp"
#include "afl/sys/environment.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace sys {

    /** Long option command line parser.
        This implements a command line parser that only supports long options.

        - a single or double dash introduces a long option, e.g. "--foo" or "-foo".
          A parameter can be given using "=" or in the next argument, e.g. "--foo=bar", "--foo bar".
        - "-" is a nonoption, "--" treats everything that follows as a nonoption.

        Otherwise, options and nonoptions can be mixed. */

    class LongCommandLineParser : public CommandLineParser {
     public:
        /** Constructor.
            \param commandLine Command line to parse */
        LongCommandLineParser(afl::base::Ptr<Environment::CommandLine_t> commandLine);

        ~LongCommandLineParser();

        // CommandLineParser:
        virtual bool getNext(bool& option, String_t& text);
        virtual bool getParameter(String_t& value);
        virtual Flags_t getFlags();

     private:
        afl::base::Ptr<Environment::CommandLine_t> m_commandLine;

        enum State {
            Idle,               // Must fetch a new element
            Word,               // Got "--xyz".
            WordParam,          // Got "--xyz=foo". m_currentValue is "foo".
            End                 // Got "--", parsing nonoptions.
        };
        State m_state;

        String_t m_currentValue;
    };

} }

#endif
