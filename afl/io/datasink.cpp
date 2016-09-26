/**
  *  \file afl/io/datasink.cpp
  *  \brief Interface afl::io::DataSink
  */

#include "afl/io/datasink.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"

void
afl::io::DataSink::handleFullData(const String_t& name, afl::base::ConstBytes_t data)
{
    if (handleData(name, data)) {
        if (!data.empty()) {
            throw afl::except::FileProblemException(name, afl::string::Messages::cannotWrite());
        }
    }
}
