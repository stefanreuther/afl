/**
  *  \file afl/string/parsememory.cpp
  *  \brief String Parsers for ConstStringMemory_t objects
  */

#include "afl/string/parsememory.hpp"

size_t
afl::string::parseIf(ConstStringMemory_t& mem, bool pred(char))
{
    size_t n = 0;
    while (const char* pch = mem.at(n)) {
        if (pred(*pch)) {
            ++n;
        } else {
            break;
        }
    }

    mem.split(n);
    return n;
}

size_t
afl::string::parseChar(ConstStringMemory_t& mem, char ch)
{
    size_t n = 0;
    if (const char* pch = mem.at(0)) {
        if (*pch == ch) {
            ++n;
            mem.split(1);
        }
    }
    return n;
}

size_t
afl::string::parseLiteral(ConstStringMemory_t& mem, const char* lit)
{
    size_t n = 0;
    while (lit[n] != '\0') {
        const char* pch = mem.at(n);
        if (pch == 0 || *pch != lit[n]) {
            return 0;
        }
        ++n;
    }
    mem.split(n);
    return n;
}

size_t
afl::string::parseNumber(ConstStringMemory_t& mem, uint64_t& out)
{
    size_t n = 0;
    uint64_t result = 0;
    while (const char* pch = mem.at(n)) {
        if (*pch >= '0' && *pch <= '9') {
            uint64_t newVal = 10*result + (*pch - '0');
            if (newVal < result) {
                return 0;
            }
            result = newVal;
            ++n;
        } else {
            break;
        }
    }
    if (n > 0) {
        mem.split(n);
        out = result;
    }
    return n;
}
