/**
  *  \file afl/sys/commandlineparser.hpp
  *  \brief Interface afl::sys::CommandLineParser
  */
#ifndef AFL_AFL_SYS_COMMANDLINEPARSER_HPP
#define AFL_AFL_SYS_COMMANDLINEPARSER_HPP

#include "afl/base/deletable.hpp"
#include "afl/bits/smallset.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace sys {

    /** Command line parser.
        This is the basic interface.
        Derived classes will implement actual parsing (typically of a Environment::CommandLine_t).

        Users call getNext() to obtain a command line item.
        getNext() returns an option or nonoption, if any.
        For each item, you can call
        - getFlags() to obtain additional information about the option.
        - getParameter() once to obtain a possible parameter. The parser may or may not know whether there was a parameter. */
    class CommandLineParser : public afl::base::Deletable {
     public:
        enum Flag {
            /** We know that there is a parameter. For example, "--foo=bar".
                If the option was spelled "--foo bar" or "-Fbar", we do not know. */
            HasParameter,

            /** This was a long option. For example, "--foo". */
            IsLongOption
        };
        typedef afl::bits::SmallSet<Flag> Flags_t;

        /** Fetch an option.
            \param option [out] true if we found an option, false if we found a nonoption
            \param text   [out] option or text name
            \retval true we found another item
            \retval false we did not find another item, command line ends */
        virtual bool getNext(bool& option, String_t& text) = 0;

        /** Get parameter.
            If the option was already known to have a parameter ("--foo=bar", HasParameter flag), returns that.
            Otherwise, consumes a possible following value and returns that.

            This function can be called once per option, after getNext().

            \param value [out] Parameter value
            \retval true Parameter value found
            \retval false Parameter value not found */
        virtual bool getParameter(String_t& value) = 0;

        /** Get flags.
            These are optional flags associated with each command line item.
            If you call this, call it after getNext(), before getParameter().
            \return flags */
        virtual Flags_t getFlags() = 0;

        /** Get a required parameter.
            If the parameter is not specified, throws a afl::except::CommandLineException.
            This is a convenience wrapper around getParameter().
            \param text Name of option as produced by getNext() */
        String_t getRequiredParameter(const String_t& text);
    };

} }

#endif
