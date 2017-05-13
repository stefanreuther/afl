/**
  *  \file afl/string/win32filenames.cpp
  *  \brief Class afl::string::Win32FileNames
  *
  *  \todo UNC support is completely missing here
  */

#include <list>
#include "afl/string/win32filenames.hpp"

const char afl::string::Win32FileNames::PATH_SEPARATOR;

void
afl::string::Win32FileNames::flipSlashes(FileName_t& name) const
{
    String_t::size_type n = 0;
    while ((n = name.find('/', n)) != name.npos) {
        name[n] = PATH_SEPARATOR;
    }
}

bool
afl::string::Win32FileNames::isAbsolutePathName(FileName_t path) const
{
    return (path.size() > 2 && path[1] == ':' && (path[2] == '\\' || path[2] == '/'));
}

bool
afl::string::Win32FileNames::isPathSeparator(char c) const
{
    return c == '\\' || c == ':' || c == '/';
}

afl::string::Win32FileNames::FileName_t
afl::string::Win32FileNames::makePathName(FileName_t path, FileName_t name) const
{
    if (isAbsolutePathName(name) || (name.size() >= 2 && name[1] == ':') || (name.size() > 0 && (name[0] == '/' || name[0] == '\\'))) {
        // absolute ("c:/foo"), drive-relative ("c:foo"), or root-relative ("\foo")
        return name;
    } else {
        // relative
        if (path.size() != 0 && !isPathSeparator(path[path.size()-1])) {
            path += PATH_SEPARATOR;
        }
        return path + name;
    }
}

afl::string::Win32FileNames::FileName_t
afl::string::Win32FileNames::getCanonicalPathName(FileName_t name) const
{
    flipSlashes(name);

    // If there is a drive letter, remove it and save it for later. */
    FileName_t prefix;
    if (name.size() >= 1 && name[1] == ':') {
        prefix.assign(name, 0, 2);
        name.erase(0, 2);
    }

    // Convert remainder
    bool isAbsolute = false;
    size_t upCount = 0;
    std::list<String_t> out;

    // Check leading slash
    FileName_t::size_type pos = 0;
    if (name[0] == PATH_SEPARATOR) {
        isAbsolute = true;
        pos = 1;
    }

    while (pos < name.size()) {
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
            // normal
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
    return prefix + s;
}

afl::string::Win32FileNames::FileName_t
afl::string::Win32FileNames::getFileName(FileName_t name) const
{
    FileName_t::size_type d = name.find_last_of("\\:/");
    if (d != name.npos && (name[d] == '\\' || name[d] == '/' || d == 1)) {
        // "fooo\bar", "foo/bar", or "a:" (but not "bla:blub" which is a file with a stream)
        return name.substr(d+1);
    } else {
        // Just a file name
        return name;
    }
}

afl::string::Win32FileNames::FileName_t
afl::string::Win32FileNames::getDirectoryName(FileName_t name) const
{
    flipSlashes(name);
    FileName_t::size_type d = name.find_last_of("\\:");
    if (d == 0 && name[d] == '\\') {
        /* `\file' */
        return FileName_t(1, PATH_SEPARATOR);
    } else if (d == 1 && name[1] == ':') {
        /* `a:file' */
        return name.substr(0, 2);
    } else if (d == 2 && name[1] == ':' && name[2] == '\\') {
        /* `a:\file' */
        return name.substr(0, 3);
    } else if (d != FileName_t::npos && name[d] == '\\') {
        /* `dir\file' */
        return name.substr(0, d);
    } else {
        /* `file' */
        return ".";
    }
}
