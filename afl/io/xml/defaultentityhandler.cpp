/**
  *  \file afl/io/xml/defaultentityhandler.cpp
  */

#include <cstdlib>
#include "afl/io/xml/defaultentityhandler.hpp"
#include "afl/charset/utf8.hpp"

String_t
afl::io::xml::DefaultEntityHandler::expandEntityReference(const String_t name)
{
    using afl::charset::Utf8;
    using afl::charset::Unichar_t;
    if (name.size() == 0) {
        return String_t();
    } else if (name[0] == '#') {
        String_t result;
        if (name.size() > 2 && name[1] == 'x') {
            Utf8().append(result, static_cast<Unichar_t>(std::strtoul(name.c_str()+2, 0, 16)));
        } else {
            Utf8().append(result, static_cast<Unichar_t>(std::strtoul(name.c_str()+1, 0, 10)));
        }
        return result;
    } else if (name == "lt") {
        return "<";
    } else if (name == "gt") {
        return ">";
    } else if (name == "apos") {
        return "\'";
    } else if (name == "quot") {
        return "\"";
    } else if (name == "amp") {
        return "&";
    } else if (name == "nbsp") {
        return "\xC2\xA0";
    } else {
        return name;
    }
}

afl::io::xml::DefaultEntityHandler&
afl::io::xml::DefaultEntityHandler::getInstance()
{
    static DefaultEntityHandler instance;
    return instance;
}
