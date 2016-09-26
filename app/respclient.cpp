/**
  *  \file app/respclient.cpp
  *  \brief Sample application: RESP client
  *
  *  Invoke as "respclient host:port command..." to invoke a command through a RESP service,
  *  i.e. redis, c2host, etc.
  */

#include <iostream>
#include <stdexcept>
#include "afl/sys/environment.hpp"
#include "afl/net/name.hpp"
#include "afl/data/segment.hpp"
#include "afl/net/resp/client.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/io/json/writer.hpp"

int main(int /*argc*/, char** argv)
{
    // Fetch command line
    afl::base::Ptr<afl::sys::Environment::CommandLine_t> cmdl =
        afl::sys::Environment::getInstance(argv).getCommandLine();

    // First element is network address
    String_t str;
    if (!cmdl->getNextElement(str)) {
        std::cout << "Missing network address.\n";
        return 1;
    }
    afl::net::Name name = afl::net::Name::parse(str, "6666");

    // Following elements are command
    afl::data::Segment cmd;
    while (cmdl->getNextElement(str)) {
        cmd.pushBack(str);
    }
    if (cmd.size() == 0) {
        std::cout << "Missing command.\n";
        return 1;
    }

    // Do it!
    try {
        // Call it
        afl::net::resp::Client client(afl::net::NetworkStack::getInstance(), name);
        std::auto_ptr<afl::data::Value> result(client.call(cmd));
        
        // Print output as JSON
        afl::io::InternalSink sink;
        afl::io::json::Writer(sink).visit(result.get());
        std::cout << "result: " << afl::string::fromBytes(sink.getContent()) << "\n";

        return 0;
    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "unknown exception\n";
        return 0;
    }
}
