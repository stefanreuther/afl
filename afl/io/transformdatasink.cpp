/**
  *  \file afl/io/transformdatasink.cpp
  *  \brief Class afl::io::TransformDataSink
  */

#include "afl/io/transformdatasink.hpp"
#include "afl/io/transform.hpp"

afl::io::TransformDataSink::TransformDataSink(DataSink& other)
    : DataSink(),
      m_pTransform(),
      m_other(other)
{ }

afl::io::TransformDataSink::~TransformDataSink()
{ }

void
afl::io::TransformDataSink::setNewTransform(Transform* pTransform)
{
    m_pTransform.reset(pTransform);
}

bool
afl::io::TransformDataSink::handleData(const String_t& name, afl::base::ConstBytes_t& data)
{
    if (m_pTransform.get() == 0) {
        // Trivial (uninitialized) case
        return m_other.handleData(name, data);
    } else {
        // Normal case
        uint8_t buffer[4096];
        bool result = false;
        while (!data.empty()) {
            afl::base::Bytes_t out(buffer);
            m_pTransform->transform(data, out);
            if (!out.empty()) {
                // FIXME: if the sink actually reports completion, this will still continue
                // transforming data, pushing that into the sink, relying on it to keep reporting completion.
                // In the future, we could hold back data, and possibly allow the user to plug in a different sink.
                afl::base::ConstBytes_t in(out);
                result = m_other.handleData(name, in);
            }
        }
        return result;
    }
}
