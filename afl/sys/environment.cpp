/**
  *  \file afl/sys/environment.cpp
  *  \brief Class afl::sys::Environment
  */

#include "afl/sys/environment.hpp"
#include "arch/environment.hpp"

// Attach standard I/O channel as TextWriter, don't throw.
afl::base::Ptr<afl::io::TextWriter>
afl::sys::Environment::attachTextWriterNT(Channel ch)
{
    try {
        return attachTextWriter(ch);
    }
    catch (...) {
        return 0;
    }
}

// Attach standard I/O channel as TextReader, don't throw.
afl::base::Ptr<afl::io::TextReader>
afl::sys::Environment::attachTextReaderNT(Channel ch)
{
    try {
        return attachTextReader(ch);
    }
    catch (...) {
        return 0;
    }
}

// Attach standard I/O stream as Stream, don't throw.
afl::base::Ptr<afl::io::Stream>
afl::sys::Environment::attachStreamNT(Channel ch)
{
    try {
        return attachStream(ch);
    }
    catch (...) {
        return 0;
    }
}

// Get environment instance.
afl::sys::Environment&
afl::sys::Environment::getInstance(const char*const* argv)
{
    static Environment_t instance(argv);
    return instance;
}
