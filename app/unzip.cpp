/**
  *  \file app/unzip.cpp
  *  \brief Sample Application: ZIP file access
  */

#include <iostream>
#include "afl/except/fileproblemexception.hpp"
#include "afl/sys/environment.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/string/format.hpp"
#include "afl/io/textwriter.hpp"
#include "afl/io/archive/zipreader.hpp"
#include "afl/io/directoryentry.hpp"

int main(int, char** argv)
{
    using afl::base::Ref;
    using afl::base::Ptr;
    using afl::io::archive::ZipReader;
    using afl::sys::Environment;
    using afl::io::FileSystem;
    using afl::io::Stream;
    using afl::io::DirectoryEntry;

    try {
        // Get environment
        Environment& env = Environment::getInstance(argv);
        Ref<Environment::CommandLine_t> cmdl = env.getCommandLine();
        Ref<afl::io::TextWriter> out = env.attachTextWriter(Environment::Output);

        // Get zip file name
        String_t fileName;
        if (!cmdl->getNextElement(fileName)) {
            std::cout << "usage: " << argv[0] << " FILE.zip [MEMBER...]\n";
            return 0;
        }

        // Open zip file
        FileSystem& fs = FileSystem::getInstance();
        Ref<ZipReader> zip = ZipReader::open(fs.openFile(fileName, FileSystem::OpenRead), 0);

        // Extract members
        String_t memberName;
        bool did = false;
        while (cmdl->getNextElement(memberName)) {
            Ref<Stream> in = zip->openFile(memberName, FileSystem::OpenRead);
            Ref<Stream> out = fs.openFile(memberName, FileSystem::Create);
            out->copyFrom(*in);
            did = true;
        }

        // If no members were specified, list content
        if (!did) {
            Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > content = zip->getDirectoryEntries();
            Ptr<DirectoryEntry> entry;
            while (content->getNextElement(entry)) {
                out->writeLine(entry->getTitle());
                out->writeLine(afl::string::Format("  %s bytes", entry->getFileSize()));

                afl::sys::Time t = entry->getModificationTime();
                if (t.isValid()) {
                    out->writeLine(afl::string::Format("  %s", t.toString(t.LocalTime, t.FullFormat)));
                }

                switch (entry->getFileType()) {
                 case DirectoryEntry::tUnknown:
                    out->writeLine("  Unknown");
                    break;
                 case DirectoryEntry::tFile:
                    out->writeLine("  File");
                    break;
                 case DirectoryEntry::tDirectory:
                    out->writeLine("  Directory");
                    break;
                 case DirectoryEntry::tArchive:
                    out->writeLine("  Archive");
                    break;
                 case DirectoryEntry::tRoot:
                    out->writeLine("  Root");
                    break;
                 case DirectoryEntry::tDevice:
                    out->writeLine("  Device");
                    break;
                 case DirectoryEntry::tOther:
                    out->writeLine("  Other");
                    break;
                }
            }
        }
    }
    catch (afl::except::FileProblemException& fpe) {
        std::cerr << "exception: " << fpe.getFileName() << ": " << fpe.what() << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
    }
}
