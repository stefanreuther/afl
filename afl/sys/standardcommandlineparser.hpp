/**
  *  \file afl/sys/standardcommandlineparser.hpp
  *  \brief Class afl::sys::StandardCommandLineParser
  */
#ifndef AFL_AFL_SYS_STANDARDCOMMANDLINEPARSER_HPP
#define AFL_AFL_SYS_STANDARDCOMMANDLINEPARSER_HPP

#include "afl/sys/commandlineparser.hpp"
#include "afl/sys/environment.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace sys {

    /** Standard command line parser.
        This implements pretty standard "getopt_long-style" command line parsing.

        - a single dash introduces a sequence of short options, e.g. "-abc" (same as "-a -b -c").
          A short option can be treated as taking a parameter (e.g. "-abc" = "-a bc").
        - a double dash introduces a long option, e.g. "--foo".
          A parameter can be given using "=" or in the next argument, e.g. "--foo=bar", "--foo bar".
        - "-" is a nonoption, "--" treats everything that follows as a nonoption.

        Otherwise, options and nonoptions can be mixed. */
    class StandardCommandLineParser : public CommandLineParser {
     public:
        /** Constructor.
            \param commandLine Command line to parse */
        StandardCommandLineParser(afl::base::Ptr<Environment::CommandLine_t> commandLine);

        ~StandardCommandLineParser();

        // CommandLineParser:
        virtual bool getNext(bool& option, String_t& text);
        virtual bool getParameter(String_t& value);
        virtual Flags_t getFlags();

     private:
        afl::base::Ptr<Environment::CommandLine_t> m_commandLine;

        enum State {
            Idle,               // Must fetch a new element
            Characters,         // Got "-xyz", parsing these. m_currentValue is "-xyz", m_currentIndex is position.
            Word,               // Got "--xyz".
            WordParam,          // Got "--xyz=foo". m_currentValue is "foo".
            End                 // Got "--", parsing nonoptions.
        };
        State m_state;

        String_t m_currentValue;
        String_t::size_type m_currentIndex;
    };

} }

#endif
