/**
  *  \file test/afl/io/filemappingtest.cpp
  *  \brief Test for afl::io::FileMapping
  */

#include "afl/io/filemapping.hpp"

#include "afl/io/directory.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests for file mapping. */
AFL_TEST("afl.io.FileMapping", a)
{
    using afl::base::Ptr;
    using afl::base::Ref;
    using afl::io::FileMapping;

    static const char FILENAME[] = "__test.tmp";
    static const size_t PAGE = 4096;

    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    try {
        {
            // Create a file
            Ref<afl::io::Stream> s = dir->openFile(FILENAME, fs.Create);
            for (int i = 1; i <= 10; ++i) {
                uint8_t buffer[PAGE];
                afl::base::Bytes_t(buffer).fill(uint8_t(i));
                s->fullWrite(buffer);
            }
        }
        {
            // Test 1: map two pages
            Ref<afl::io::Stream> s = dir->openFile(FILENAME, fs.OpenRead);
            Ptr<FileMapping> map = s->createFileMapping(2*PAGE);
            a.check("01. createFileMapping", map.get() != 0);
            afl::base::ConstBytes_t content = map->get();
            a.checkEqual("02. content.size", content.size(), 2*PAGE);

            // File pointer must be after two pages
            a.checkEqual("03. getPos", s->getPos(), 2*PAGE);

            // First page must contain ones
            for (size_t i = 0; i < PAGE; ++i) {
                const uint8_t* p = content.eat();
                a.check("11. content", p);
                a.check("12. content", *p == 1);
            }

            // Second page must contain twos
            for (size_t i = 0; i < PAGE; ++i) {
                const uint8_t* p = content.eat();
                a.check("21. content", p);
                a.check("22. content", *p == 2);
            }
            a.check("23. content end", content.empty());
        }
        {
            // Test 2: map two pages, with offset
            Ref<afl::io::Stream> s = dir->openFile(FILENAME, fs.OpenRead);
            s->setPos(1000);
            Ptr<FileMapping> map = s->createFileMapping(2*PAGE);
            a.check("31. createFileMapping", map.get() != 0);
            afl::base::ConstBytes_t content = map->get();
            a.checkEqual("32. content.size", content.size(), 2*PAGE);

            // File pointer must be after two pages
            a.checkEqual("33. getPos", s->getPos(), 1000 + 2*PAGE);

            // First partial page must contain ones
            for (size_t i = 0; i < PAGE - 1000; ++i) {
                const uint8_t* p = content.eat();
                a.check("41. content", p);
                a.check("42. content", *p == 1);
            }

            // Second page must contain twos
            for (size_t i = 0; i < PAGE; ++i) {
                const uint8_t* p = content.eat();
                a.check("51. content", p);
                a.check("52. content", *p == 2);
            }

            // Third partial page must contain threes
            for (size_t i = 0; i < 1000; ++i) {
                const uint8_t* p = content.eat();
                a.check("61. content", p);
                a.check("62. content", *p == 3);
            }
            a.check("63. content end", content.empty());
        }
        {
            // Test 3: map whole file
            Ptr<afl::io::Stream> s = dir->openFile(FILENAME, fs.OpenRead).asPtr();
            Ptr<FileMapping> map = s->createFileMapping();
            a.check("71. createFileMapping", map.get() != 0);
            afl::base::ConstBytes_t content = map->get();
            a.checkEqual("72. content.size", content.size(), 10*PAGE);

            // File pointer must be at end
            a.checkEqual("73. getPos", s->getPos(), 10*PAGE);

            // Closing the stream does not invalidate the memory
            s = 0;

            // Check all pages
            for (int val = 1; val <= 10; ++val) {
                for (size_t i = 0; i < PAGE; ++i) {
                    const uint8_t* p = content.eat();
                    a.check("74. content", p);
                    a.check("75. content", *p == val);
                }
            }
            a.check("76. content.end", content.empty());
        }
        dir->eraseNT(FILENAME);
    }
    catch (...) {
        dir->eraseNT(FILENAME);
        throw;
    }
}
