/**
  *  \file afl/string/posixfilenames.cpp
  *  \brief Class afl::string::PosixFileNames
  */

#include <list>
#include "afl/string/posixfilenames.hpp"

const char afl::string::PosixFileNames::PATH_SEPARATOR;

bool
afl::string::PosixFileNames::isAbsolutePathName(FileName_t path) const
{
    return path.size() > 0 && path[0] == PATH_SEPARATOR;
}

bool
afl::string::PosixFileNames::isPathSeparator(char c) const
{
    return c == PATH_SEPARATOR;
}

afl::string::PosixFileNames::FileName_t
afl::string::PosixFileNames::makePathName(FileName_t path, FileName_t name) const
{
    // Note: unlike the PCC2 version of this function,
    // this is not guaranteed to return a canonical name.
    if (isAbsolutePathName(name)) {
        return name;
    } else {
        if (path.size() != 0 && !isPathSeparator(path[path.size()-1])) {
            path += PATH_SEPARATOR;
        }
        return path + name;
    }
}

afl::string::PosixFileNames::FileName_t
afl::string::PosixFileNames::getCanonicalPathName(FileName_t name) const
{
    bool isAbsolute = false;
    size_t upCount = 0;
    std::list<FileName_t> out;

    // Check leading slsh
    FileName_t::size_type pos = 0;
    if (name.size() > 0 && name[0] == PATH_SEPARATOR) {
        isAbsolute = true;
        pos = 1;
    }

    // Process components
    while (pos < name.length()) {
        // Find next delimiter
        FileName_t::size_type next = name.find(PATH_SEPARATOR, pos);
        if (next == FileName_t::npos) {
            next = name.size();
        }

        // Process
        if (next == pos) {
            // ignore consecutive slashes
        } else if (next == pos+1 && name[pos] == '.') {
            // ignore "." components
        } else if (next == pos+2 && name.compare(pos, 2, "..", 2) == 0) {
            // ".." component
            if (!out.empty()) {
                out.pop_back();
            } else {
                ++upCount;
            }
        } else {
            // normal component
            out.push_back(FileName_t(name, pos, next-pos));
        }

        if (next >= name.size()) {
            break;
        }
        pos = next+1;
    }

    // Now generate output
    FileName_t s;
    if (isAbsolute) {
        s += PATH_SEPARATOR;
    } else {
        while (upCount--) {
            s += "..";
            s += PATH_SEPARATOR;
        }
    }
    for (std::list<FileName_t>::iterator i = out.begin(); i != out.end(); ++i) {
        if (i != out.begin()) {
            s += PATH_SEPARATOR;
        }
        s += *i;
    }
    if (s.empty()) {
        s = ".";
    }
    return s;
}

afl::string::PosixFileNames::FileName_t
afl::string::PosixFileNames::getFileName(FileName_t name) const
{
    FileName_t::size_type n = name.rfind(PATH_SEPARATOR);
    if (n != FileName_t::npos) {
        return name.substr(n + 1);
    } else {
        return name;
    }
}

afl::string::PosixFileNames::FileName_t
afl::string::PosixFileNames::getDirectoryName(FileName_t name) const
{
    FileName_t::size_type n = name.rfind(PATH_SEPARATOR);
    if (n == 0) {
        /* "/file" -> "/" */
        return FileName_t(1, PATH_SEPARATOR);
    } else if (n != FileName_t::npos) {
        /* "dir/file" => "dir" */
        return name.substr(0, n);
    } else {
        /* "file" => "." */
        return ".";
    }
}
