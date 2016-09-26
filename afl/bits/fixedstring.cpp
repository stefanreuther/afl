/**
  *  \file afl/bits/fixedstring.cpp
  *  \brief Class afl::bits::FixedString
  */

#include "afl/bits/fixedstring.hpp"

String_t
afl::bits::unpackFixedString(afl::base::ConstBytes_t mem)
{
    const uint8_t* p = mem.unsafeData();
    const size_t n = mem.size();

    // Locate first null byte
    size_t len = 0;
    while (len < n && p[len] != '\0') {
        ++len;
    }

    // Trim trailing spaces
    while (len > 0 && p[len-1] == ' ') {
        --len;
    }

    // Produce result
    // FIXME: better notation possible?
    return String_t(static_cast<const char*>(static_cast<const void*>(p)), len);
}

void
afl::bits::packFixedString(afl::base::Bytes_t mem, afl::string::ConstStringMemory_t src)
{
    // Copy initial part
    size_t copied = mem.copyFrom(src.toBytes()).size();

    // Fill remaining part
    mem.subrange(copied).fill(' ');
}
