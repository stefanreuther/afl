/**
  *  \file afl/data/segment.cpp
  *  \brief Class afl::data::Segment
  */

#include <cassert>
#include "afl/data/segment.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"

afl::data::Segment::Segment()
    : m_content()
{ }

afl::data::Segment::~Segment()
{ }

// Array interface
afl::data::Value*
afl::data::Segment::operator[](Index_t index) const
{
    if (index < m_content.size()) {
        return m_content[index];
    } else {
        return 0;
    }
}

void
afl::data::Segment::set(Index_t index, Value* value)
{
    setNew(index, Value::cloneOf(value));
}

void
afl::data::Segment::setNew(Index_t index, Value* value)
{
    // Make index accessible. This might throw.
    try {
        makeIndexAccessible(index);
    }
    catch (...) {
        delete value;
        throw;
    }

    // Replace the element. This will not throw, and even if it did,
    // it would be PtrVector's responsibility to clean up ("...New" function).
    m_content.replaceElementNew(index, value);
}

void
afl::data::Segment::insertNew(Index_t index, Value* value)
{
    // Make index accessible. This might throw.
    if (index > 0) {
        try {
            makeIndexAccessible(index-1);
        }
        catch (...) {
            delete value;
            throw;
        }
    }

    // Replace the element. This will not throw, and even if it did,
    // it would be PtrVector's responsibility to clean up ("...New" function).
    m_content.insertNew(m_content.begin() + index, value);
}

afl::data::Value*
afl::data::Segment::extractElement(Index_t index)
{
    return index < m_content.size()
        ? m_content.extractElement(index)
        : 0;
}

void
afl::data::Segment::swap(Segment& other)
{
    m_content.swap(other.m_content);
}

void
afl::data::Segment::swapElements(Index_t index, Segment& other, Index_t otherIndex)
{
    makeIndexAccessible(index);
    other.makeIndexAccessible(otherIndex);
    m_content.swapElements(index, other.m_content, otherIndex);
}

void
afl::data::Segment::clear()
{
    afl::container::PtrVector<Value>().swap(m_content);
}

afl::data::Segment::Index_t
afl::data::Segment::size() const
{
    return m_content.size();
}

afl::data::Segment::Index_t
afl::data::Segment::getNumUsedSlots()
{
    Index_t result = m_content.size();
    while (result > 0 && m_content[result-1] == 0) {
        --result;
    }
    return result;
}

// Stack interface
afl::data::Segment&
afl::data::Segment::pushBack(Value* value)
{
    return pushBackNew(Value::cloneOf(value));
}

afl::data::Segment&
afl::data::Segment::pushBackInteger(int32_t value)
{
    return pushBackNew(new IntegerValue(value));
}

afl::data::Segment&
afl::data::Segment::pushBackString(const String_t& value)
{
    return pushBackNew(new StringValue(value));
}

afl::data::Segment&
afl::data::Segment::pushBackString(const char* value)
{
    return pushBackNew(new StringValue(value));
}

afl::data::Segment&
afl::data::Segment::pushBackNew(Value* value)
{
    m_content.pushBackNew(value);
    return *this;
}

void
afl::data::Segment::popBack()
{
    assert(m_content.size() > 0);
    return m_content.popBack();
}

void
afl::data::Segment::popBackN(Offset_t count)
{
    assert(m_content.size() >= count);
    for (Offset_t i = 0; i < count; ++i) {
        m_content.popBack();
    }
}

afl::data::Value*
afl::data::Segment::top(Offset_t ofs) const
{
    assert(ofs < m_content.size());
    return m_content[m_content.size() - ofs - 1];
}

afl::data::Value*
afl::data::Segment::top() const
{
    assert(m_content.size() > 0);
    return m_content.back();
}

afl::data::Value*
afl::data::Segment::extractTop()
{
    assert(m_content.size() > 0);
    return m_content.extractLast();
}

void
afl::data::Segment::transferLastTo(Offset_t count, Segment& other)
{
    assert(count <= m_content.size());

    other.m_content.reserve(other.m_content.size() + count);

    // Copy elements
    Index_t pos = m_content.size() - count;
    for (Index_t i = 0; i < count; ++i) {
        other.pushBackNew(m_content.extractElement(pos + i));
    }

    // Release empty slots
    popBackN(count);
}

void
afl::data::Segment::makeIndexAccessible(Index_t index)
{
    if (index >= m_content.size()) {
        m_content.reserve(index+1);
        while (index >= m_content.size()) {
            m_content.pushBackNew(0);
        }
    }
}
