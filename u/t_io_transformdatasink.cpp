/**
  *  \file u/t_io_transformdatasink.cpp
  *  \brief Test for afl::io::TransformDataSink
  */

#include "afl/io/transformdatasink.hpp"

#include "u/t_io.hpp"
#include "afl/io/datasink.hpp"
#include "afl/string/string.hpp"
#include "afl/io/transform.hpp"

namespace {
    // A test sink that accumulates data in a string
    class TestSink : public afl::io::DataSink {
     public:
        TestSink(String_t& text)
            : m_text(text)
            { }
        virtual bool handleData(afl::base::ConstBytes_t& data)
            {
                m_text.append(reinterpret_cast<const char*>(data.unsafeData()), data.size());
                data.reset();
                return false;
            }

     private:
        String_t& m_text;
    };

    // A test transformation that replicates each input byte five times
    class TestTransform : public afl::io::Transform {
     public:
        TestTransform()
            : m_state(0),
              m_byte(0)
            { }

        virtual void transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out)
            {
                size_t outIndex = 0;
                while (outIndex < out.size()) {
                    if (m_state == 0) {
                        if (const uint8_t* p = in.eat()) {
                            m_byte = *p;
                        } else {
                            break;
                        }
                    }
                    *out.at(outIndex) = m_byte;
                    ++outIndex;
                    ++m_state;
                    if (m_state >= 5) {
                        m_state = 0;
                    }
                }
                out.trim(outIndex);
            }

        virtual void flush()
            { }

     private:
        int m_state;
        uint8_t m_byte;
    };
}

/** Test behaviour with a null transformation (pass-through). */
void
TestIoTransformDataSink::testNull()
{
    // Setup
    String_t text;
    TestSink tester(text);
    afl::io::TransformDataSink sink(tester);

    // Perform operation
    afl::base::ConstBytes_t bytes(afl::string::toBytes("abc"));
    bool result = sink.handleData(bytes);

    // Check invocation result
    TS_ASSERT(!result);
    TS_ASSERT(bytes.empty());

    // Check result
    TS_ASSERT_EQUALS(text, "abc");
}

/** Simple test. */
void
TestIoTransformDataSink::testSimple()
{
    // Setup
    String_t text;
    TestSink tester(text);
    afl::io::TransformDataSink sink(tester);
    sink.setNewTransform(new TestTransform());

    // Perform operation
    afl::base::ConstBytes_t bytes(afl::string::toBytes("abc"));
    bool result = sink.handleData(bytes);

    // Check invocation result
    TS_ASSERT(!result);
    TS_ASSERT(bytes.empty());

    // Check result
    TS_ASSERT_EQUALS(text, "aaaaabbbbbccccc");
    TS_ASSERT_EQUALS(text.size(), 15U);
}

/** Test large data that hits the TransformDataSink buffer boundary. */
void
TestIoTransformDataSink::testLarge()
{
    // Make a buffer
    // As of 20151003, TransformDataSink's buffer is 4k, so 8k is guaranteed to hit the buffer boundary.
    uint8_t buf[8192];
    afl::base::Bytes_t(buf).fill('x');

    for (size_t sz = 1; sz <= sizeof(buf); sz <<= 1) {
        // Setup
        String_t text;
        TestSink tester(text);
        afl::io::TransformDataSink sink(tester);
        sink.setNewTransform(new TestTransform());

        // Stuff (5x)64k in it
        for (size_t i = 0; i*sz < 65536U; ++i) {
            afl::base::ConstBytes_t bytes(buf);
            bytes.trim(sz);

            sink.handleData(bytes);
            TS_ASSERT_EQUALS(text.size(), (i+1)*5*sz);
        }
    }
}
