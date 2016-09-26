/**
  *  \file u/t_io_bufferedstream.cpp
  *  \brief Test for afl::io::BufferedStream
  */

#include "afl/io/bufferedstream.hpp"

#include "u/t_io.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/memorystream.hpp"
#include "afl/string/string.hpp"

/** Test readByte, peekByte. */
void
TestIoBufferedStream::testReadByte()
{
    afl::io::ConstMemoryStream cms(afl::string::toBytes("123\012abc"));
    afl::io::BufferedStream bs(cms);

    // 1
    const uint8_t* p;
    p = bs.peekByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, '1');

    p = bs.readByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, '1');

    // 2
    p = bs.peekByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, '2');

    p = bs.readByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, '2');

    // 3
    p = bs.peekByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, '3');

    p = bs.readByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, '3');

    // newline
    p = bs.peekByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 10);

    p = bs.readByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 10);

    // a
    p = bs.peekByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 'a');

    p = bs.readByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 'a');

    // b
    p = bs.peekByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 'b');

    p = bs.readByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 'b');

    // c
    p = bs.peekByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 'c');

    p = bs.readByte();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(*p, 'c');

    // EOF
    TS_ASSERT(bs.peekByte() == 0);
    TS_ASSERT(bs.readByte() == 0);
    TS_ASSERT(bs.peekByte() == 0);
    TS_ASSERT(bs.readByte() == 0);
    TS_ASSERT(bs.peekByte() == 0);
    TS_ASSERT(bs.readByte() == 0);
}

/** Test mixed (read/write) I/O. */
void
TestIoBufferedStream::testMixedIO()
{
    {
        uint8_t mem[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                          'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                          's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
        afl::io::MemoryStream ms(mem);
        afl::io::BufferedStream bs(ms);
        TS_ASSERT_EQUALS(bs.getPos(), 0U);
        TS_ASSERT_EQUALS(bs.getSize(), 26U);

        const uint8_t* pc;
        pc = bs.readByte();
        TS_ASSERT(pc != 0);
        TS_ASSERT_EQUALS(*pc, 'a');
        pc = bs.readByte();
        TS_ASSERT(pc != 0);
        TS_ASSERT_EQUALS(*pc, 'b');
        TS_ASSERT_EQUALS(bs.getPos(), 2U);

        static const uint8_t CD[] = { 'C', 'D' };
        TS_ASSERT_EQUALS(bs.write(CD), 2U);
        TS_ASSERT_EQUALS(bs.getPos(), 4U);

        pc = bs.readByte();
        TS_ASSERT(pc != 0);
        TS_ASSERT_EQUALS(*pc, 'e');
        pc = bs.readByte();
        TS_ASSERT(pc != 0);
        TS_ASSERT_EQUALS(*pc, 'f');

        bs.setPos(3);
        TS_ASSERT_EQUALS(bs.getPos(), 3U);
        pc = bs.readByte();
        TS_ASSERT(pc != 0);
        TS_ASSERT_EQUALS(*pc, 'D');

        uint8_t tmp[5];
        TS_ASSERT_EQUALS(bs.read(tmp), 5U);
        TS_ASSERT_SAME_DATA(tmp, "efghi", 5);

        bs.setPos(24);
        TS_ASSERT_EQUALS(bs.read(tmp), 2U);
        TS_ASSERT_SAME_DATA(tmp, "yzghi", 5);

        bs.flush();
        TS_ASSERT_SAME_DATA(mem, "abCDefg", 7);
    }
}
