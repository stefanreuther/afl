/**
  *  \file afl/io/stream.cpp
  *  \brief Class afl::io::Stream
  */

#include "afl/io/stream.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/io/internalfilemapping.hpp"
#include "afl/io/filemapping.hpp"

void
afl::io::Stream::fullRead(Bytes_t m)
{
    while (!m.empty()) {
        size_t n = read(m);
        if (n == 0) {
            throw afl::except::FileTooShortException(*this);
        }
        m.split(n);
    }
}

void
afl::io::Stream::fullWrite(ConstBytes_t m)
{
    while (!m.empty()) {
        size_t n = write(m);
        if (n == 0) {
            throw afl::except::FileProblemException(*this, afl::string::Messages::cannotWrite());
        }
        m.split(n);
    }
}

bool
afl::io::Stream::hasCapabilities(uint32_t which)
{
    return (getCapabilities() & which) == which;
}

void
afl::io::Stream::copyFrom(Stream& other)
{
    uint8_t buffer[4096];
    while (1) {
        Bytes_t m(buffer);
        m.trim(other.read(m));
        if (m.empty()) {
            break;
        }
        fullWrite(m);
    }
}

void
afl::io::Stream::copyFrom(Stream& other, FileSize_t size)
{
    uint8_t buffer[4096];
    while (size > 0) {
        Bytes_t m(buffer);
        if (size < m.size()) {
            m.trim(size_t(size));
        }
        other.fullRead(m);
        fullWrite(m);
        size -= m.size();
    }
}

afl::base::Ref<afl::io::FileMapping>
afl::io::Stream::createVirtualMapping(FileSize_t limit)
{
    afl::base::Ptr<FileMapping> mapping = createFileMapping(limit);
    if (mapping.get() == 0) {
        return *new InternalFileMapping(*this, limit);
    } else {
        return *mapping;
    }
}

bool
afl::io::Stream::handleData(afl::base::ConstBytes_t& data)
{
    data.split(write(data));
    return !data.empty();
}
