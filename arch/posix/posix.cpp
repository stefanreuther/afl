/**
  *  \file arch/posix/posix.cpp
  *  \brief POSIX specifics
  */

#include <clocale>
#if TARGET_OS_POSIX

#include <wchar.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "arch/posix/posix.hpp"
#include "afl/charset/utf8.hpp"
#include "afl/charset/unicode.hpp"
#include "afl/charset/utf8reader.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"

// Convert external string to UTF-8 for internal use.
String_t
arch::posix::convertExternalToUtf8(afl::string::ConstStringMemory_t in)
{
    String_t result;
    afl::charset::Utf8 u8;

    setupLocale();

    mbstate_t mbs;
    memset(&mbs, 0, sizeof(mbs));

    while (!in.empty()) {
        wchar_t wc;
        size_t processed = mbrtowc(&wc, in.unsafeData(), in.size(), &mbs);
        if (processed == 0) {
            // Null byte. Pretend the end.
            break;
        } else if (processed > in.size() || wchar_t(afl::charset::Unichar_t(wc)) != wc || afl::charset::isErrorCharacter(wc)) {
            // Error case. Either the character is invalid, or not representable.
            // "processed > size" catches the regular error returns (-1, -2)
            // as well as other random malfunctions.
            if (const char* pc = in.eat()) {
                u8.append(result, afl::charset::makeErrorCharacter(*pc));
            }
        } else {
            // Regular case
            in.split(processed);
            u8.append(result, wc);
        }
    }
    return result;
}

// Convert UTF-8 string to external string.
bool
arch::posix::convertUtf8ToExternal(String_t& result, afl::string::ConstStringMemory_t in, ConversionMode mode)
{
    setupLocale();

    mbstate_t mbs;
    memset(&mbs, 0, sizeof(mbs));

    afl::charset::Utf8Reader rdr(in.toBytes(), 0);
    while (rdr.hasMore()) {
        afl::charset::Unichar_t ch = rdr.eat();
        if (afl::charset::isErrorCharacter(ch)) {
            // It's an error character; keep it.
            result += char(afl::charset::getErrorCharacterId(ch));
        } else if (ch == 0) {
            // Null byte
            switch (mode) {
             case RelaxedConversion:
                result += '\0';
                break;
             case ParanoidConversion:
                return false;
            }
        } else {
            // Regular Unicode character
            char tmp[MB_LEN_MAX];
            size_t produced = wcrtomb(tmp, ch, &mbs);
            if (produced > sizeof(tmp)) {
                // We cannot encode this character.
                switch (mode) {
                 case RelaxedConversion:
                    result += '?';
                    break;
                 case ParanoidConversion:
                    return false;
                }
            } else {
                result.append(tmp, produced);
            }
        }
    }
    return true;
}

// Convert UTF-8 string to path name.
String_t
arch::posix::convertUtf8ToPathName(const String_t in)
{
    String_t result;
    if (!convertUtf8ToExternal(result, afl::string::toMemory(in), ParanoidConversion)) {
        throw afl::except::FileProblemException(in, afl::string::Messages::invalidFileName());
    }
    return result;
}

void
arch::posix::setupLocale()
{
    static bool beenHere = false;
    if (!beenHere) {
        std::setlocale(LC_CTYPE, "");
        std::setlocale(LC_MESSAGES, "");
    }
}

bool
arch::posix::readlinkWrap(const char* fileName, afl::base::GrowableMemory<char>& buffer)
{
    // readlink() truncates if the buffer is too small.
    // We detect that if it returns precisely the buffer size.
    // We still limit the result arbitrarily to avoid that we explode if anything goes wrong.
    const ssize_t MIN_BUFFER = 128;
    const ssize_t MAX_BUFFER = 0x3FFF;

    buffer.resize(MIN_BUFFER);
    ssize_t n;
    while ((n = ::readlink(fileName, buffer.unsafeData(), buffer.size())) == int(buffer.size()) && n < MAX_BUFFER) {
        buffer.resize(2*buffer.size());
    }
    if (n > 0) {
        buffer.resize(n);
        return true;
    } else {
        return false;
    }
}
#else
int g_variableToMakePosixObjectFileNotEmpty;
#endif
