/**
  *  \file afl/data/segmentview.cpp
  *  \brief Class afl::data::SegmentView
  */

#include <algorithm>
#include "afl/data/segmentview.hpp"
#include "afl/data/access.hpp"
#include "afl/data/segment.hpp"

// Constructor for whole segment.
afl::data::SegmentView::SegmentView(const Segment& seg)
    : m_segment(seg),
      m_index(0),
      m_count(seg.size())
{ }

// Constructor for partial segment.
afl::data::SegmentView::SegmentView(const Segment& seg, size_t first, size_t limit)
    : m_segment(seg),
      m_index(std::min(seg.size(), first)),
      m_count(std::min(seg.size() - m_index, limit))
{ }

// Get number of elements remaining in the SegmentView.
size_t
afl::data::SegmentView::size() const
{
    return m_count;
}

// Read one value.
afl::data::Value*
afl::data::SegmentView::eat()
{
    if (m_count > 0) {
        --m_count;
        return m_segment[m_index++];
    } else {
        return 0;
    }
}

// Read a string value.
bool
afl::data::SegmentView::eat(String_t& value)
{
    if (Value* val = eat()) {
        value = Access(val).toString();
        return true;
    } else {
        return false;
    }
}

// Read an integer value.
bool
afl::data::SegmentView::eat(int32_t& value)
{
    if (Value* val = eat()) {
        value = Access(val).toInteger();
        return true;
    } else {
        return false;
    }
}
