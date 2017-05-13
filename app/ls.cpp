/**
  *  \file app/ls.cpp
  *  \brief Sample Application: List Directories
  */

#include <iostream>
#include <cstring>
#include "afl/io/filesystem.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/format.hpp"
#include "afl/io/textwriter.hpp"
#include "afl/sys/environment.hpp"

int main(int /*argc*/, char** argv)
{
    using afl::base::Ptr;
    using afl::base::Ref;
    using afl::io::FileSystem;
    using afl::io::Directory;
    using afl::io::DirectoryEntry;
    using afl::sys::Environment;

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
            Ref<Directory> dir = (arg == "--root") ? FileSystem::getInstance().openRootDirectory() : FileSystem::getInstance().openDirectory(arg);
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
