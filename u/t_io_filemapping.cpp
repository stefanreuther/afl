/**
  *  \file u/t_io_filemapping.cpp
  *  \brief Test for afl::io::FileMapping
  */

#include "afl/io/filemapping.hpp"

#include "u/t_io.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/stream.hpp"

/** Simple tests for file mapping. */
void
TestIoFileMapping::testIt()
{
    using afl::base::Ptr;
    using afl::io::FileMapping;
    
    static const char FILENAME[] = "__test.tmp";
    static const size_t PAGE = 4096;

    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    Ptr<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    try {
        {
            // Create a file
            Ptr<afl::io::Stream> s = dir->openFile(FILENAME, fs.Create);
            for (int i = 1; i <= 10; ++i) {
                uint8_t buffer[PAGE];
                afl::base::Bytes_t(buffer).fill(uint8_t(i));
                s->fullWrite(buffer);
            }
        }
        {
            // Test 1: map two pages
            Ptr<afl::io::Stream> s = dir->openFile(FILENAME, fs.OpenRead);
            Ptr<FileMapping> map = s->createFileMapping(2*PAGE);
            TS_ASSERT(map.get() != 0);
            afl::base::ConstBytes_t content = map->get();
            TS_ASSERT_EQUALS(content.size(), 2*PAGE);

            // File pointer must be after two pages
            TS_ASSERT_EQUALS(s->getPos(), 2*PAGE);

            // First page must contain ones
            for (size_t i = 0; i < PAGE; ++i) {
                const uint8_t* p = content.eat();
                TS_ASSERT(p);
                TS_ASSERT(*p == 1);
            }

            // Second page must contain twos
            for (size_t i = 0; i < PAGE; ++i) {
                const uint8_t* p = content.eat();
                TS_ASSERT(p);
                TS_ASSERT(*p == 2);
            }
            TS_ASSERT(content.empty());
        }
        {
            // Test 2: map two pages, with offset
            Ptr<afl::io::Stream> s = dir->openFile(FILENAME, fs.OpenRead);
            s->setPos(1000);
            Ptr<FileMapping> map = s->createFileMapping(2*PAGE);
            TS_ASSERT(map.get() != 0);
            afl::base::ConstBytes_t content = map->get();
            TS_ASSERT_EQUALS(content.size(), 2*PAGE);

            // File pointer must be after two pages
            TS_ASSERT_EQUALS(s->getPos(), 1000 + 2*PAGE);

            // First partial page must contain ones
            for (size_t i = 0; i < PAGE - 1000; ++i) {
                const uint8_t* p = content.eat();
                TS_ASSERT(p);
                TS_ASSERT(*p == 1);
            }

            // Second page must contain twos
            for (size_t i = 0; i < PAGE; ++i) {
                const uint8_t* p = content.eat();
                TS_ASSERT(p);
                TS_ASSERT(*p == 2);
            }

            // Third partial page must contain threes
            for (size_t i = 0; i < 1000; ++i) {
                const uint8_t* p = content.eat();
                TS_ASSERT(p);
                TS_ASSERT(*p == 3);
            }
            TS_ASSERT(content.empty());
        }
        {
            // Test 3: map whole file
            Ptr<afl::io::Stream> s = dir->openFile(FILENAME, fs.OpenRead);
            Ptr<FileMapping> map = s->createFileMapping();
            TS_ASSERT(map.get() != 0);
            afl::base::ConstBytes_t content = map->get();
            TS_ASSERT_EQUALS(content.size(), 10*PAGE);

            // File pointer must be at end
            TS_ASSERT_EQUALS(s->getPos(), 10*PAGE);

            // Closing the stream does not invalidate the memory
            s = 0;

            // Check all pages
            for (int val = 1; val <= 10; ++val) {
                for (size_t i = 0; i < PAGE; ++i) {
                    const uint8_t* p = content.eat();
                    TS_ASSERT(p);
                    TS_ASSERT(*p == val);
                }
            }
            TS_ASSERT(content.empty());
        }
        dir->eraseNT(FILENAME);
    }
    catch (...) {
        dir->eraseNT(FILENAME);
        throw;
    }
}
