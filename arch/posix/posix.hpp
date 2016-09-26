/**
  *  \file arch/posix/posix.hpp
  *  \brief POSIX specifics
  */
#ifndef AFL_ARCH_POSIX_POSIX_HPP
#define AFL_ARCH_POSIX_POSIX_HPP

#if TARGET_OS_POSIX
#include "afl/string/string.hpp"
#include "afl/base/growablememory.hpp"

namespace arch { namespace posix {

    /** Convert external string to UTF-8 for internal use.
        \param in Externally-provided string
        \return converted string */
    String_t convertExternalToUtf8(afl::string::ConstStringMemory_t in);

    /** Convert UTF-8 string to external string.
        \param result [out] Result goes here
        \param in [in] UTF-8 string
        \param placeholders [in] true to use placeholders for unencodable characters, false to return an error
        \retval true conversion succeeded
        \retval false conversion error, and placeholders was false */
    bool convertUtf8ToExternal(String_t& result,
                               afl::string::ConstStringMemory_t in,
                               bool placeholders);

    /** Convert UTF-8 string to path name.
        If the conversion fails, reports a FileProblemException, as if the file open failed.
        \param in UTF-8 string
        \return converted file name */
    String_t convertUtf8ToPathName(const String_t in);

    void setupLocale();

    bool readlinkWrap(const char* fileName, afl::base::GrowableMemory<char>& buffer);

} }
#else
# error Invalid operating system
#endif

#endif
