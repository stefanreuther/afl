/**
  *  \file afl/net/commandhandler.hpp
  *  \brief Interface afl::net::CommandHandler
  */
#ifndef AFL_AFL_NET_COMMANDHANDLER_HPP
#define AFL_AFL_NET_COMMANDHANDLER_HPP

#include "afl/base/deletable.hpp"
#include "afl/data/value.hpp"
#include "afl/data/segment.hpp"
#include "afl/base/types.hpp"
#include "afl/string/string.hpp"
#include "afl/base/optional.hpp"

namespace afl { namespace net {

    /** Interface for a command interpreter.
        This interface allows executing commands with positional arguments, typically to a remote system.
        Such commands could be shell commands, line-based protocols like FTP, or machine-readable commands such as RESP.
        Commands with named arguments, such as for web-based commands, can be executed by passing a hash of parameters.

        Although typically a command consists of a command word in the form of a string in the first position,
        and scalar (integer/string) parameters in the following positions, the interface imposes no such limit.
        It is up to the actual protocol to define limits.

        The interface implements two functions:
        - call() has a return value
        - callVoid() has no return value
        The intention is to allow reduction of network roundtrips (pipelining) or other optimisations for the callVoid() case.
        Actual protocols can decide whether particular commands can use just one form or both interchangeably.
        The minimum implementation implements either of these functions in terms of the other
        ("call" as "callVoid" + return 0, or "callVoid" as "delete call"). */
    class CommandHandler : public afl::base::Deletable {
    public:
        /** Shortcut for afl::data::Value. */
        typedef afl::data::Value Value_t;

        /** Shortcut for afl::data::Segment. */
        typedef afl::data::Segment Segment_t;

        /*
         *  Interface methods
         */

        /** Invoke a command, returning a value.
            \param command Command and arguments as a list of values.
            \throw afl::except::RemoteErrorException The remote end signalled an error.
            \throw afl::except::InvalidDataException The remote end sent invalid data (protocol error).
            \return New value, can be null. Caller assumes ownership.
            Note that this function should by default never return an error value; it should throw instead. */
        virtual Value_t* call(const Segment_t& command) = 0;

        /** Invoke a command, returning no value.
            \param command Command and arguments as a list of values.
            \throw afl::except::RemoteErrorException The remote end signalled an error.
            \throw afl::except::InvalidDataException The remote end sent invalid data (protocol error). */
        virtual void callVoid(const Segment_t& command) = 0;


        /*
         *  Derived methods
         */

        /** Invoke a command, returning an integer value.
            The result of the call is interpreted as an integer.
            If it cannot be converted, an afl::except::InvalidDataException is signalled.
            \param command Command and arguments as a list of values.
            \throw afl::except::RemoteErrorException The remote end signalled an error.
            \throw afl::except::InvalidDataException The remote end sent invalid data (protocol error).
            \return result */
        int32_t callInt(const Segment_t& command);

        /** Invoke a command, returning an optional integer value.
            The result of the call is interpreted as an integer.
            If the value does not exist (null), Nothing is returned.
            If it exists but cannot be converted, an afl::except::InvalidDataException is signalled.
            \param command Command and arguments as a list of values.
            \throw afl::except::RemoteErrorException The remote end signalled an error.
            \throw afl::except::InvalidDataException The remote end sent invalid data (protocol error).
            \return result */
        afl::base::Optional<int32_t> callOptionalInt(const Segment_t& command);

        /** Invoke a command, returning a string value.
            The result of the call is interpreted as a string.
            If it cannot be converted, an afl::except::InvalidDataException is signalled.
            \param command Command and arguments as a list of values.
            \throw afl::except::RemoteErrorException The remote end signalled an error.
            \throw afl::except::InvalidDataException The remote end sent invalid data (protocol error).
            \return result */
        String_t callString(const Segment_t& command);
    };

} }

#endif
