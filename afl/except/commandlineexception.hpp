/**
  *  \file afl/except/commandlineexception.hpp
  *  \brief Class afl::except::CommandLineException
  */
#ifndef AFL_AFL_EXCEPT_COMMANDLINEEXCEPTION_HPP
#define AFL_AFL_EXCEPT_COMMANDLINEEXCEPTION_HPP

#include <stdexcept>
#include "afl/string/string.hpp"

namespace afl { namespace except {

    /** Problem with the command line.
        This exception is thrown when a command line parser sees a problem. */
    class CommandLineException : public std::runtime_error {
     public:
        /** Construct from file name and message.
            \param msg Message, not ending in a period or newline */
        CommandLineException(const String_t& msg);
    };


} }

#endif
