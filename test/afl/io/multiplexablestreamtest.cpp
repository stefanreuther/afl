/**
  *  \file test/afl/io/multiplexablestreamtest.cpp
  *  \brief Test for afl::io::MultiplexableStream
  */

#include "afl/io/multiplexablestream.hpp"

#include <stdexcept>
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
    static uint8_t data[] = { 1,2,3,4,5 };
    afl::io::MemoryStream ms(data);
    a.checkEqual("size of stream", ms.getSize(), 5U);
    ms.setPos(2);

    // Create children
    afl::base::Ref<afl::io::Stream> one = ms.createChild();
    afl::base::Ref<afl::io::Stream> two = ms.createChild();
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
                static uint8_t data[] = { 1,2,3,4,5 };
                afl::io::MemoryStream ms(data);
                afl::base::Ptr<afl::io::Stream> children[COUNT] = {
                    ms.createChild().asPtr(),
                    ms.createChild().asPtr(),
                    ms.createChild().asPtr(),
                    ms.createChild().asPtr()
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
    afl::base::Ref<afl::io::Stream> child(t.createChild());
    child->setPos(23);
}
