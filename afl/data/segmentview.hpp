/**
  *  \file afl/data/segmentview.hpp
  *  \brief Class afl::data::SegmentView
  */
#ifndef AFL_AFL_DATA_SEGMENTVIEW_HPP
#define AFL_AFL_DATA_SEGMENTVIEW_HPP

#include "afl/string/string.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace data {

    class Segment;
    class Value;

    /** View into a Segment.
        Allows you to interpret content from a list of values stored in a Segment.
        It is configured with a range of a segment.
        Repeated calls to eat() extract the contained values.
        Attempting to read after the end of the view will return null values.

        (This class is similar in spirit to afl::base::Memory, but with a simpler/more specialized interface.) */
    class SegmentView {
     public:
        /** Constructor for whole segment.
            Make a view that spans the whole segment.
            \param seg Segment, must have sufficient lifetime. */
        explicit SegmentView(const Segment& seg);

        /** Constructor for partial segment.
            Make a view that spans part of a segment.
            If the given boundaries are out of range, they will be clamped into valid range.
            \param seg Segment, must have sufficient lifetime.
            \param first Index into segment for first element to extract
            \param limit Maximum number of elements to extract */
        SegmentView(const Segment& seg, size_t first, size_t limit);

        /** Get number of elements remaining in the SegmentView.
            \return number of elements */
        size_t size() const;

        /** Read one value.
            \return raw value (owned by the underlying Segment), 0 if end of view reached */
        Value* eat();

        /** Read a string value.
            Consumes one value and converts it to a string.
            \param value [out] string value stored here
            \retval true Value could be converted successfully
            \retval false Segment contained a null value, \c value has not been modified
            \throw afl::except::InvalidDataException if value cannot be converted to string */
        bool eat(String_t& value);

        /** Read an integer value.
            Consumes one value and converts it to an integer.
            \param value [out] integer value stored here
            \retval true Value could be converted successfully
            \retval false Segment contained a null value, \c value has not been modified
            \throw afl::except::InvalidDataException if value cannot be converted to an integer */
        bool eat(int32_t& value);

     private:
        const Segment& m_segment;
        size_t m_index;
        size_t m_count;
    };

} }

#endif
