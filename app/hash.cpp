/**
  *  \file app/hash.cpp
  *  \brief Sample application: Hashing
  *
  *  Invoke as
  *    hash -type file...
  *  e.g.
  *    hash -md5 file.dat
  *  This will compute the given hash and output it to standard output,
  *  and will produce the same output as "md5sum", "sha1sum", etc.
  */

#include <memory>
#include <cstdlib>
#include "afl/checksums/hash.hpp"
#include "afl/checksums/md5.hpp"
#include "afl/checksums/sha1.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/string/format.hpp"
#include "afl/sys/environment.hpp"
#include "afl/checksums/sha256.hpp"
#include "afl/checksums/sha224.hpp"
#include "afl/checksums/sha384.hpp"
#include "afl/checksums/sha512.hpp"

namespace {
    void hashStream(afl::checksums::Hash* pHash,
                    afl::io::Stream& in,
                    afl::io::TextWriter& out)
    {
        // Do we have a hash?
        if (pHash == 0) {
            out.writeLine("No hash configured");
            out.flush();
            std::exit(1);
        }

        // Read the file
        pHash->clear();
        while (1) {
            uint8_t buffer[4096];
            afl::base::Bytes_t bufferDescriptor(buffer);
            bufferDescriptor.trim(in.read(bufferDescriptor));
            if (bufferDescriptor.empty()) {
                break;
            }
            pHash->add(bufferDescriptor);
        }

        // Get output
        out.writeLine(afl::string::Format("%s  %s", pHash->getHashAsHexString(), in.getName()));
    }
}

int main(int, char** argv)
{
    // Environment
    afl::sys::Environment& env = afl::sys::Environment::getInstance(argv);
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::TextWriter> out(env.attachTextWriter(env.Output));

    // Current hash
    std::auto_ptr<afl::checksums::Hash> hash;

    // Operate
    try {
        afl::base::Ref<afl::sys::Environment::CommandLine_t> cmdl(env.getCommandLine());
        String_t what;
        while (cmdl->getNextElement(what)) {
            if (what == "-") {
                hashStream(hash.get(), *env.attachStream(env.Input), *out);
            } else if (what == "-md5") {
                hash.reset(new afl::checksums::MD5());
            } else if (what == "-sha1") {
                hash.reset(new afl::checksums::SHA1());
            } else if (what == "-sha224") {
                hash.reset(new afl::checksums::SHA224());
            } else if (what == "-sha256") {
                hash.reset(new afl::checksums::SHA256());
            } else if (what == "-sha384") {
                hash.reset(new afl::checksums::SHA384());
            } else if (what == "-sha512") {
                hash.reset(new afl::checksums::SHA512());
            } else if (what == "" || what[0] == '-') {
                out->writeLine(afl::string::Format("Unknown command line parameter: \"%s\"", what));
            } else {
                hashStream(hash.get(), *fs.openFile(what, fs.OpenRead), *out);
            }
        }
        out->flush();
    }
    catch (afl::except::FileProblemException& e) {
        out->writeLine(afl::string::Format("%s: %s", e.getFileName(), e.what()));
        out->flush();
    }
    catch (std::exception& e) {
        out->writeLine(afl::string::Format("exception: %s", e.what()));
        out->flush();
    }
}
