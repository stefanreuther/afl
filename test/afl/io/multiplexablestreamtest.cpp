/**
  *  \file test/afl/io/multiplexablestreamtest.cpp
  *  \brief Test for afl::io::MultiplexableStream
  */

#include "afl/io/multiplexablestream.hpp"

#include <stdexcept>
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/io/memorystream.hpp"
#include "afl/test/testrunner.hpp"

// We'll be using a MemoryStream as sample implementation.
// Make sure it actually is derived from MultiplexableStream.
AFL_TEST_NOARG("afl.io.MultiplexableStream:MemoryStream-is-derived")
{
    afl::io::MemoryStream* ms = 0;
    afl::io::MultiplexableStream* muxs = ms;
    (void) muxs;
}

// Test parallel access
AFL_TEST("afl.io.MultiplexableStream:parallel-access", a)
{
    // Preinitialize
    uint8_t data[] = { 1,2,3,4,5 };
    afl::io::MemoryStream ms(data);
    a.checkEqual("size of stream", ms.getSize(), 5U);
    ms.setPos(2);

    // Create children
    afl::base::Ref<afl::io::Stream> one = ms.createChild(0);
    afl::base::Ref<afl::io::Stream> two = ms.createChild(0);
    a.checkNonNull("address one", &one.get());
    a.checkNonNull("address two", &two.get());

    a.checkEqual("size one", one->getSize(), 5U);
    a.checkEqual("size two", two->getSize(), 5U);

    a.checkEqual("pos one", one->getPos(), 0U);
    a.checkEqual("pos two", two->getPos(), 0U);

    // Access children
    uint8_t tmp[2];
    size_t n = two->read(tmp);
    a.checkEqual("read from two", n, 2U);
    a.checkEqual("pos two changed", two->getPos(), 2U);
    a.checkEqual("pos one unchanged", one->getPos(), 0U);
    a.checkEqual("byte 0 from two", tmp[0], 1U);
    a.checkEqual("byte 1 from two", tmp[1], 2U);

    uint8_t tmp2[3];
    n = one->read(tmp2);
    a.checkEqual("read from one", n, 3U);
    a.checkEqual("pos one changed", one->getPos(), 3U);
    a.checkEqual("pos two unchanged", two->getPos(), 2U);
    a.checkEqual("byte 0 from one", tmp2[0], 1U);
    a.checkEqual("byte 1 from one", tmp2[1], 2U);
    a.checkEqual("byte 2 from one", tmp2[2], 3U);
}

/** Test lifetime management. */
AFL_TEST_NOARG("afl.io.MultiplexableStream:lifetime")
{
    // Try releasing streams in different orders.
    const size_t COUNT = 4;
    for (size_t access = 0; access < COUNT+1; ++access) {
        for (size_t freeFirst = 0; freeFirst < COUNT; ++freeFirst) {
            for (size_t freeSecond = 0; freeSecond < COUNT; ++freeSecond) {
                // Create it
                uint8_t data[] = { 1,2,3,4,5 };
                afl::io::MemoryStream ms(data);
                afl::base::Ptr<afl::io::Stream> children[COUNT] = {
                    ms.createChild(0).asPtr(),
                    ms.createChild(0).asPtr(),
                    ms.createChild(0).asPtr(),
                    ms.createChild(0).asPtr()
                };

                // Access and thus activate one
                if (access < COUNT) {
                    uint8_t tmp[1];
                    children[access]->read(tmp);
                }

                // Release in order
                children[freeFirst] = 0;
                children[freeSecond] = 0;
            }
        }
    }
}

// Test setPos() behaviour.
// If the first operation on a new stream is setPos(), only one setPos() call should be done.
AFL_TEST("afl.io.MultiplexableStream:setPos", a)
{
    class Tester : public afl::io::MultiplexableStream {
     public:
        Tester(afl::test::Assert a)
            : m_assert(a)
            { }
        virtual size_t read(Bytes_t /*m*/)
            { throw std::runtime_error("unexpected"); }
        virtual size_t write(ConstBytes_t /*m*/)
            { throw std::runtime_error("unexpected"); }
        virtual void flush()
            { throw std::runtime_error("unexpected"); }
        virtual void setPos(FileSize_t pos)
            { m_assert.checkEqual("setPos parameter", pos, 23U); }
        virtual FileSize_t getPos()
            { return 42; }
        virtual FileSize_t getSize()
            { throw std::runtime_error("unexpected"); }
        virtual uint32_t getCapabilities()
            { return 0; }
        virtual String_t getName()
            { return "<name>"; }
        virtual afl::base::Ptr<afl::io::FileMapping> createFileMapping(FileSize_t /*limit*/)
            { throw std::runtime_error("unexpected"); }
     private:
        afl::test::Assert m_assert;
    };
    Tester t(a);
    afl::base::Ref<afl::io::Stream> child(t.createChild(0));
    child->setPos(23);
}

// Test mode limitation
AFL_TEST("afl.io.MultiplexableStream:mode-limit", a)
{
    // Preinitialize
    uint8_t data[] = { 1,2,3,4,5 };
    afl::io::MemoryStream ms(data);
    a.checkEqual("size of stream", ms.getSize(), 5U);

    // Create children
    afl::base::Ref<afl::io::Stream> one = ms.createChild(0);
    afl::base::Ref<afl::io::Stream> two = ms.createChild(afl::io::Stream::DisableWrite);
    afl::base::Ref<afl::io::Stream> three = two->createChild(0);

    // Capability check
    a.checkEqual("read capa one",    one->getCapabilities()   & afl::io::Stream::CanRead,  afl::io::Stream::CanRead);
    a.checkEqual("write capa one",   one->getCapabilities()   & afl::io::Stream::CanWrite, afl::io::Stream::CanWrite);
    a.checkEqual("read capa two",    two->getCapabilities()   & afl::io::Stream::CanRead,  afl::io::Stream::CanRead);
    a.checkEqual("write capa two",   two->getCapabilities()   & afl::io::Stream::CanWrite, 0U);
    a.checkEqual("read capa three",  three->getCapabilities() & afl::io::Stream::CanRead,  afl::io::Stream::CanRead);
    a.checkEqual("write capa three", three->getCapabilities() & afl::io::Stream::CanWrite, 0U);

    // Attempt to write into 'one'
    static const uint8_t w1[] = { 77,88 };
    a.checkEqual("write one", one->write(w1), 2U);

    // Attempt to write onto 'two'
    static const uint8_t w2[] = { 33,44 };
    AFL_CHECK_THROWS(a("write two"), two->write(w2), afl::except::FileProblemException);

    // Attempt to write onto 'three'
    static const uint8_t w3[] = { 55,66 };
    AFL_CHECK_THROWS(a("write three"), three->write(w3), afl::except::FileProblemException);

    // Verify data
    a.checkEqual("data 0", data[0], 77);
    a.checkEqual("data 1", data[1], 88);
    a.checkEqual("data 2", data[2], 3);
    a.checkEqual("data 3", data[3], 4);
    a.checkEqual("data 4", data[4], 5);
}

// Test mode limitation
AFL_TEST("afl.io.MultiplexableStream:mapping", a)
{
    // Preinitialize
    uint8_t data[] = { 1,2,3,4,5 };
    afl::io::MemoryStream ms(data);
    a.checkEqual("size of stream", ms.getSize(), 5U);

    // Create children
    afl::base::Ref<afl::io::Stream> one = ms.createChild(0);
    afl::base::Ref<afl::io::FileMapping> map = one->createVirtualMapping();

    a.checkEqual("data 0", *map->get().at(0), 1);
    a.checkEqual("data 1", *map->get().at(1), 2);
    a.checkEqual("data 2", *map->get().at(2), 3);
    a.checkEqual("data 3", *map->get().at(3), 4);
    a.checkEqual("data 4", *map->get().at(4), 5);
}
