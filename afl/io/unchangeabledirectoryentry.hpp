/**
  *  \file afl/io/unchangeabledirectoryentry.hpp
  *  \brief afl::io::UnchangeableDirectoryEntry
  */
#ifndef AFL_AFL_IO_UNCHANGEABLEDIRECTORYENTRY_HPP
#define AFL_AFL_IO_UNCHANGEABLEDIRECTORYENTRY_HPP

#include "afl/io/directoryentry.hpp"

namespace afl { namespace io {

    /** Base class for an unchangeable DirectoryEntry.
        This provides default methods for a DirectoryEntry that provides a view on unchangeable data.
        This includes archives, remote data, and other virtualized content.
        Methods that attempt to modify the content are rejected with an appropriate afl::except::FileProblemException.
        These are:
        - rename(), erase(), setFlag(), createAsDirectory()
        - openFile() for anything other than reading

        You still need to implement remaining methods that provide read access. */
    class UnchangeableDirectoryEntry : public DirectoryEntry {
     public:
        /** Constructor.
            \param errorMessage Error message to use for exceptions. Sensible values are
                                afl::string::Messages::cannotWrite(), afl::string::Messages::cannotModifyArchiveFile(). */
        explicit UnchangeableDirectoryEntry(String_t errorMessage);

        /*
         *  Public DirectoryEntry methods to implement by derived class:
         */
        virtual String_t getTitle() = 0;
        virtual String_t getPathName() = 0;
        virtual afl::base::Ref<Directory> openDirectory() = 0;
        virtual afl::base::Ref<Directory> openContainingDirectory() = 0;

        /*
         *  Implemented public DirectoryEntry methods
         */

        /** Open file.
            If anything other than read access is requested, refuses the request.
            Otherwise, calls openFileForReading().
            \param mode Open mode
            \return Stream, never null
            \throw FileProblemException if there is a problem */
        virtual afl::base::Ref<Stream> openFile(FileSystem::OpenMode mode);

     protected:
        /** Open file for reading.
            This is called by openFile() if opening for reading is requested.
            \return Stream, never null
            \throw FileProblemException if there is a problem */
        virtual afl::base::Ref<Stream> openFileForReading() = 0;

        /*
         *  Protected DirectoryEntry methods to implement by derived class:
         */
        virtual void updateInfo(uint32_t requested) = 0;

        /*
         *  Implemented protected DirectoryEntry methods
         *
         *  These are all implemented to produce an error message.
         */
        virtual void doRename(String_t newName);
        virtual void doErase();
        virtual void doCreateAsDirectory();
        virtual void doSetFlag(FileFlag flag, bool value);
        virtual void doMoveTo(Directory& dir, String_t name);

     private:
        void fail();

        String_t m_errorMessage;
    };

} }

inline
afl::io::UnchangeableDirectoryEntry::UnchangeableDirectoryEntry(String_t errorMessage)
    : DirectoryEntry(),
      m_errorMessage(errorMessage)
{ }

#endif
