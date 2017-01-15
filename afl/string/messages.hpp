/**
  *  \file afl/string/messages.hpp
  *  \brief Class afl::string::Messages
  */
#ifndef AFL_AFL_STRING_MESSAGES_HPP
#define AFL_AFL_STRING_MESSAGES_HPP

#include "afl/string/translator.hpp"

namespace afl { namespace string {

    /** System strings and messages.
        This class operates as a container to system messages.
        It provides a number of functions to access the messages.
        This ensures consistency:
        - a message used at multiple times reads identical all the time
        - all messages are run through the system translator

        It also makes sure that the object code of a program only contains the
        minimum required number of messages. */
    struct Messages {
        /*
         *  Exception texts
         */

        /** Range error. Use for all kinds of range violations.
            \return message */
        static String_t rangeError();

        /** File not found. Use for all kinds of access to a nonexistant file or directory.
            \return message */
        static String_t fileNotFound();

        /** File exists. Use whenever a file exists which should not.
            \return message */
        static String_t fileExists();

        /** File too short. Use for all kinds of short reads.
            \return message */
        static String_t fileTooShort();

        /** Invalid file name. Use whenever a file name cannot be used by the current platform.
            \return message */
        static String_t invalidFileName();

        /** Invalid URL.
            \return message */
        static String_t invalidUrl();

        /** Invalid operation. Use for all kinds of invalid operations.
            An invalid operation typically cannot be triggered by end-users.
            \return message */
        static String_t invalidOperation();

        /** Number expected.
            Use whenever a number is expected but something else was provided.
            \return message */
        static String_t invalidNumber();

        /** Invalid file header.
            \return message */
        static String_t invalidFileHeader();

        /** Invalid compressed data.
            \return message */
        static String_t invalidCompressedData();

        /** Cannot access files. Use whenever someone tries to open, create or otherwise access a file
            in a place where this is not possible.
            \return message */
        static String_t cannotAccessFiles();

        /** Cannot access directories. Use whenever someone tries to open, create or otherwise access a directory
            in a place where this is not possible.
            \return message */
        static String_t cannotAccessDirectories();

        /** Cannot modify archive file.
            Use whenever someone tries to modify an archive file using a component that cannot modify archives.
            \return message */
        static String_t cannotModifyArchiveFile();

        /** Cannot write. Use for all sorts of failing writes.
            \return message */
        static String_t cannotWrite();

        /** Network name not found. Use when a connection fails.
            \return message */
        static String_t networkNameNotFound();

        /** Unspecified network error.
            \return message */
        static String_t networkError();

        /** Network connection lost. Use when a connection is closed when it shouldn't have been.
            \return message */
        static String_t networkConnectionLost();

        /** Syntax error.
            \return message */
        static String_t syntaxError();

        /** Unknown error. Use whenever the reason is not known (i.e. unknown exception).
            \return message */
        static String_t unknownError();

        /** Operation cancelled. Use as status for a cancelled operation.
            \return message */
        static String_t operationCancelled();

        /** Unsupported feature.
            \return message */
        static String_t unsupportedFeature();

        /** Unsupported compression mode.
            \return message */
        static String_t unsupportedCompressionMode();

        /** Unexpected error.
            \return message */
        static String_t unexpectedError();

        /** Assertion failed.
            \return format string with one placeholder for the assertion. */
        static String_t assertionFailed();

        /** Assertion failed at location.
            \return format string with two placeholders; one for the assertion, one for the location. */
        static String_t assertionFailedAt();

        /** Missing command line option.
            \return format string with one placeholder for the option name. */
        static String_t missingCommandLineParameter();


        /*
         *  Other texts
         */

        /** Name of root directory. This is used as title for the top of the file-system tree,
            FileSystem::openRootDirectory().
            \return message */
        static String_t rootDirTitle();

        /** Name of "My Files" link.
            \return message */
        static String_t myFilesLinkTitle();

        /** Name of "Desktop" link.
            \return message */
        static String_t desktopLinkTitle();

        /** Name of "Root directory" link.
            \return message */
        static String_t rootLinkTitle();

        /** Name of "Home directory" link.
            \return message */
        static String_t homeLinkTitle();
    };

} }


/***************************** Implementation ****************************/

inline String_t
afl::string::Messages::rangeError()
{
    return AFL_TRANSLATE_STRING("Range error");
}

inline String_t
afl::string::Messages::fileNotFound()
{
    return AFL_TRANSLATE_STRING("File not found");
}

inline String_t
afl::string::Messages::fileExists()
{
    return AFL_TRANSLATE_STRING("File exists");
}

inline String_t
afl::string::Messages::fileTooShort()
{
    return AFL_TRANSLATE_STRING("File too short");
}

inline String_t
afl::string::Messages::invalidFileName()
{
    return AFL_TRANSLATE_STRING("Invalid file name");
}

inline String_t
afl::string::Messages::invalidUrl()
{
    return AFL_TRANSLATE_STRING("Invalid or unsupported URL");
}

inline String_t
afl::string::Messages::invalidOperation()
{
    return AFL_TRANSLATE_STRING("Invalid operation");
}

inline String_t
afl::string::Messages::invalidNumber()
{
    return AFL_TRANSLATE_STRING("Invalid number");
}

inline String_t
afl::string::Messages::invalidFileHeader()
{
    return AFL_TRANSLATE_STRING("Invalid file header");
}

inline String_t
afl::string::Messages::invalidCompressedData()
{
    return AFL_TRANSLATE_STRING("Invalid compressed data");
}

inline String_t
afl::string::Messages::cannotAccessFiles()
{
    return AFL_TRANSLATE_STRING("Cannot access files here");
}

inline String_t
afl::string::Messages::cannotAccessDirectories()
{
    return AFL_TRANSLATE_STRING("Cannot access directories");
}

inline String_t
afl::string::Messages::cannotModifyArchiveFile()
{
    return AFL_TRANSLATE_STRING("Cannot modify archive file");
}

inline String_t
afl::string::Messages::cannotWrite()
{
    return AFL_TRANSLATE_STRING("Cannot write");
}

inline String_t
afl::string::Messages::networkNameNotFound()
{
    return AFL_TRANSLATE_STRING("Network name not found");
}

inline String_t
afl::string::Messages::networkError()
{
    return AFL_TRANSLATE_STRING("Network error");
}

inline String_t
afl::string::Messages::networkConnectionLost()
{
    return AFL_TRANSLATE_STRING("Network connection lost");
}

inline String_t
afl::string::Messages::syntaxError()
{
    return AFL_TRANSLATE_STRING("Syntax error");
}

inline String_t
afl::string::Messages::unknownError()
{
    return AFL_TRANSLATE_STRING("Unknown error");
}

inline String_t
afl::string::Messages::operationCancelled()
{
    return AFL_TRANSLATE_STRING("Operation cancelled");
}

inline String_t
afl::string::Messages::unsupportedFeature()
{
    return AFL_TRANSLATE_STRING("Unsupported feature");
}

inline String_t
afl::string::Messages::unsupportedCompressionMode()
{
    return AFL_TRANSLATE_STRING("Unsupported compression mode");
}

inline String_t
afl::string::Messages::unexpectedError()
{
    return AFL_TRANSLATE_STRING("Unexpected error");
}

inline String_t
afl::string::Messages::assertionFailed()
{
    return AFL_TRANSLATE_STRING("Assertion failed: %s");
}

inline String_t
afl::string::Messages::assertionFailedAt()
{
    return AFL_TRANSLATE_STRING("Assertion failed: %s, at %s");
}

inline String_t
afl::string::Messages::missingCommandLineParameter()
{
    return AFL_TRANSLATE_STRING("Option \"%s\" is missing its parameter");
}

inline String_t
afl::string::Messages::rootDirTitle()
{
    return AFL_TRANSLATE_STRING("Root");
}

inline String_t
afl::string::Messages::myFilesLinkTitle()
{
    return AFL_TRANSLATE_STRING("My Files");
}

inline String_t
afl::string::Messages::desktopLinkTitle()
{
    return AFL_TRANSLATE_STRING("Desktop");
}

inline String_t
afl::string::Messages::rootLinkTitle()
{
    return AFL_TRANSLATE_STRING("Root Directory");
}

inline String_t
afl::string::Messages::homeLinkTitle()
{
    return AFL_TRANSLATE_STRING("Home Directory");
}

#endif
