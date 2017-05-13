/**
  *  \file afl/io/datasink.cpp
  *  \brief Interface afl::io::DataSink
  */

#include "afl/io/datasink.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"

void
afl::io::DataSink::handleFullData(afl::base::ConstBytes_t data)
{
    if (handleData(data)) {
        if (!data.empty()) {
            throw afl::except::FileProblemException("<DataSink>", afl::string::Messages::cannotWrite());
        }
    }
}
