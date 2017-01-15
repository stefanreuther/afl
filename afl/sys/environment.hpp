/**
  *  \file afl/sys/environment.hpp
  *  \brief Class afl::sys::Environment
  */
#ifndef AFL_AFL_SYS_ENVIRONMENT_HPP
#define AFL_AFL_SYS_ENVIRONMENT_HPP

#include "afl/base/enumerator.hpp"
#include "afl/base/ptr.hpp"
#include "afl/string/string.hpp"
#include "afl/io/textwriter.hpp"
#include "afl/io/textreader.hpp"
#include "afl/io/stream.hpp"
#include "afl/base/ref.hpp"

namespace afl { namespace sys {

    /** Access to Environment.
        This class is a container for methods to access the environment.
        It improves on the C++ standard library by consistently supporting Unicode.

        An environment consists of:
        - environment variables
        - the command-line parameters
        - application installation parameters (directory names)
        - standard I/O channels (stdin/out/err) */
    class Environment {
     public:
        /** Standard I/O channel. */
        enum Channel {
            Input,          /**< Standard input. */
            Output,         /**< Standard output. */
            Error           /**< Standard error. */
        };

        /** Command line enumerator.
            \see getCommandLine */
        typedef afl::base::Enumerator<String_t> CommandLine_t;

        /** Get command line.
            Returns the command line in UTF-8 format.
            \return Enumerator listing all command line arguments (not including the program name) */
        virtual afl::base::Ref<CommandLine_t> getCommandLine() = 0;

        /** Get program name.
            Returns the program invocation name (argv[0]) in UTF-8.
            \return value in UTF-8. */
        virtual String_t getInvocationName() = 0;

        /** Get environment variable.
            Returns the value of the environment variable in UTF-8.
            \param name Name of the environment variable, in UTF-8 (but should normally be ASCII)
            \return value in UTF-8; empty string if variable does not exist. */
        virtual String_t getEnvironmentVariable(const String_t& name) = 0;

        /** Get name of an application's settings folder.

            \param appName Short identification of the application.

            The application identifier should be a human-readable, short, version-independent name.
            It should not contain path separators.
            It will be transformed according to target specific rules.
            For example, appName="My App" will turn into "~/.myApp" on Unix, "%APPDATA%\My App" on Windows. */
        virtual String_t getSettingsDirectoryName(const String_t& appName) = 0;

        /** Get name of installation root folder.
            Returns the folder name in UTF-8.

            POSIX: if available, returns the file name from procfs (Linux).
            Otherwise, makes a best guess from PATH, current working directory, and argv[0].
            If the result is a symlink, chases that to the binary.
            Finally, if we end up in a directory "bin", go up one level.
            Therefore, a binary "/usr/local/bin/myprog" will return "/usr/local",
            a binary "/home/user/myprog" will return "/home/user".

            Win32: returns the binary's path name, unmodified.

            \return result folder name in UTF-8. */
        virtual String_t getInstallationDirectoryName() = 0;

        /** Attach standard I/O channel as TextWriter.
            This can be used to get a textual handle to standard output or standard error.

            \todo Restrictions about whether this function should be only called more than once are still to be defined.

            POSIX: returns a TextWriter that converts UTF-8 to the system character set.

            Win32: if the given channel is a console window, writes to that using console I/O calls.
            Otherwise, returns a TextWriter that writes UTF-8 into a text file.

            \param ch Channel (should be Output or Error; using Input is a non-standard use-case that may or may not work)
            \return TextWriter, never null
            \throw std::runtime_error or FileProblemException if there is a problem */
        virtual afl::base::Ref<afl::io::TextWriter> attachTextWriter(Channel ch) = 0;

        /** Attach standard I/O channel as TextReader.
            This can be used to get a textual handle to standard input.

            \todo Restrictions about whether this function should be only called more than once are still to be defined.

            POSIX: returns a TextReader that converts the system character set to UTF-8.

            Win32: if the given channel is a console window, reads from that using console I/O calls.
            Otherwise, returns a TextReader that reads UTF-8 from a text file.

            \param ch Channel (should be Input; using Output or Error is a non-standard use-case that may or may not work)
            \return TextReader, never null
            \throw std::runtime_error or FileProblemException if there is a problem  */
        virtual afl::base::Ref<afl::io::TextReader> attachTextReader(Channel ch) = 0;

        /** Attach standard I/O stream as Stream.
            This can be used to get a binary handle to standard input, output, or error.

            \todo Restrictions about whether this function should be only called more than once are still to be defined.

            \param ch Channel
            \return Stream, never null
            \throw std::runtime_error or FileProblemException if there is a problem  */
        virtual afl::base::Ref<afl::io::Stream> attachStream(Channel ch) = 0;

        /** Attach standard I/O channel as TextWriter, don't throw.
            This function is the same as attachTextWriter, but does not throw on error.
            Instead, it will return null on error.

            \param ch Channel (should be Output or Error; using Input is a non-standard use-case that may or may not work)
            \return TextWriter or null */
        afl::base::Ptr<afl::io::TextWriter> attachTextWriterNT(Channel ch);

        /** Attach standard I/O channel as TextReader, don't throw.
            This function is the same as attachTextWriter, but does not throw on error.
            Instead, it will return null on error.

            \param ch Channel (should be Input; using Output or Error is a non-standard use-case that may or may not work)
            \return TextReader or null  */
        afl::base::Ptr<afl::io::TextReader> attachTextReaderNT(Channel ch);

        /** Attach standard I/O stream as Stream, don't throw.
            This function is the same as attachStream, but does not throw on error.

            \param ch Channel
            \return Stream or null */
        afl::base::Ptr<afl::io::Stream> attachStreamNT(Channel ch);

        /** Get environment instance.
            You must pass your argv to this function.
            This function may or may not evaluate it; if you pass anything else, results are undefined.

            The returned object has static lifetime (valid until the end of main).
            The argv you pass in must also have static lifetime.

            \param argv argv you received in your main() */
        static Environment& getInstance(const char*const* argv);
    };

} }

#endif
