/**
  *  \file app/ls.cpp
  *  \brief Sample Application: List Directories
  */

#include <iostream>
#include <cstring>
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/archive/tarreader.hpp"
#include "afl/io/archive/zipreader.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/textwriter.hpp"
#include "afl/string/format.hpp"
#include "afl/sys/environment.hpp"
#include "afl/io/inflatetransform.hpp"
#include "afl/io/transformreaderstream.hpp"

using afl::base::Ptr;
using afl::base::Ref;
using afl::io::FileSystem;
using afl::io::Directory;
using afl::io::DirectoryEntry;
using afl::sys::Environment;

namespace {
    bool endsWith(const String_t& s, const String_t& suffix)
    {
        return s.size() >= suffix.size()
            && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    Ref<Directory> openDir(const String_t& arg)
    {
        if (arg == "--root") {
            return FileSystem::getInstance().openRootDirectory();
        } else if (endsWith(arg, ".zip")) {
            return afl::io::archive::ZipReader::open(FileSystem::getInstance().openFile(arg, FileSystem::OpenRead), 0);
        } else if (endsWith(arg, ".tar")) {
            return afl::io::archive::TarReader::open(FileSystem::getInstance().openFile(arg, FileSystem::OpenRead), 0);
        } else if (endsWith(arg, ".tar.gz") || endsWith(arg, ".tgz")) {
            class InflateReader : private afl::io::InflateTransform,
                                  private Ref<afl::io::Stream>,
                                  public afl::io::TransformReaderStream
            {
             public:
                InflateReader(Ref<afl::io::Stream> in)
                    : InflateTransform(Gzip), Ref<afl::io::Stream>(in), TransformReaderStream(*in, *this)
                    { }
            };
            Ref<afl::io::TransformReaderStream> file = *new InflateReader(FileSystem::getInstance().openFile(arg, FileSystem::OpenRead));
            return afl::io::archive::TarReader::open(file, 0);
        } else {
            return FileSystem::getInstance().openDirectory(arg);
        }
    }
}


int main(int /*argc*/, char** argv)
{
    /* Fetch environment */
    Environment& env = Environment::getInstance(argv);

    /* Open stdout console. */
    afl::base::Ptr<afl::io::TextWriter> out;
    try {
        out = env.attachTextWriter(Environment::Output).asPtr();
    }
    catch (afl::except::FileProblemException& fpe) {
        std::cerr << fpe.getFileName() << ": " << fpe.what() << std::endl;
        return 1;
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    if (out.get() == 0) {
        std::cerr << "Did not get a console." << std::endl;
        return 1;
    }

    /* Do it */
    afl::base::Ref<Environment::CommandLine_t> cmdl = env.getCommandLine();
    String_t arg;
    while (cmdl->getNextElement(arg)) {
        try {
            Ref<Directory> dir = openDir(arg);
            Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > e = dir->getDirectoryEntries();
            Ptr<DirectoryEntry> entry;
            while (e->getNextElement(entry)) {
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
                if (entry->getFlags().contains(entry->Link)) {
                    out->writeLine(afl::string::Format("  Link to %s", entry->getLinkText()));
                }
                if (entry->getFlags().contains(entry->Hidden)) {
                    out->writeLine("  Hidden");
                }
                if (entry->getFlags().contains(entry->Executable)) {
                    out->writeLine("  Executable");
                }
            }
        }
        catch (afl::except::FileProblemException& fpe) {
            std::cerr << fpe.getFileName() << ": " << fpe.what() << std::endl;
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
}
