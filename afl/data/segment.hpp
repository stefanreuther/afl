/**
  *  \file afl/data/segment.hpp
  *  \brief Class afl::data::Segment
  */
#ifndef AFL_AFL_DATA_SEGMENT_HPP
#define AFL_AFL_DATA_SEGMENT_HPP

#include "afl/data/value.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/base/types.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace data {

    /** Data segment.
        This is the building block for arrays, hashes, stack frames, stacks,
        and everything else that can store multiple values. */
    class Segment {
     public:
        /** Index into array.
            Used for indexing from the beginning in the array interface. */
        typedef size_t Index_t;

        /** Number of stack elements.
            Used as offset from the end of the array in the stack interface. */
        typedef size_t Offset_t;


        /** Constructor. */
        Segment();

        /** Destructor. */
        ~Segment();


        /*
         *  Array interface
         */

        /** Get element by index.
            \param index Index to access
            \return value (owned by the Segment), or null */
        Value* operator[](Index_t index) const;

        /** Get element by index.
            \param index Index to access
            \return value (owned by the Segment), or null */
        Value* get(Index_t index) const;

        /** Set element by index.
            Ownership for the value remains at caller, even if an exception is thrown.
            The value will be cloned.
            \param index Index to access
            \param value Value to set */
        void set(Index_t index, const Value* value);

        /** Set element by index.
            Ownership for the value will be taken over by the Segment; if an exception is thrown, it is deleted.
            The value will not be cloned.
            \param index Index to access
            \param value Value to set */
        void setNew(Index_t index, Value* value);

        /** Insert new element before an index.
            The new element will be placed at the given position;
            all elements after it will be moved forward.
            \param index Index to access
            \param value Value to insert */
        void insertNew(Index_t index, Value* value);

        /** Extract element by index.
            This will set the element to null.
            \param index Index to access
            \return Value, ownership will transfer to caller */
        Value* extractElement(Index_t index);

        /** Swap content. Exchanges the context of this Segment and the other.
            \param other other segment */
        void swap(Segment& other);

        /** Swap elements. Exchanges elements of this Segment and another.
            \param index Index into this segment
            \param other Other segment
            \param otherIndex Index into other segment */
        void swapElements(Index_t index, Segment& other, Index_t otherIndex);

        /** Clear segment. Removes all content. */
        void clear();

        /** Get size.
            \return number of elements in segment */
        Index_t size() const;

        /** Get number of used slots.
            Returns the smallest possible number such that operator[] for this value
            and all greater ones will yield null (=unused or unallocated elements).
            \return number of elements */
        Index_t getNumUsedSlots() const;


        /*
         *  Stack interface
         */

        /** Push element.
            Ownership for the value remains at caller, even if an exception is thrown.
            The value will be cloned.
            \param value Value to push
            \return *this */
        Segment& pushBack(const Value* value);

        /** Push integer.
            \param value Value to push
            \return *this */
        Segment& pushBackInteger(int32_t value);

        /** Push string.
            \param value Value to push
            \return *this */
        Segment& pushBackString(const String_t& value);

        /** Push string.
            \param value Value to push
            \return *this */
        Segment& pushBackString(const char* value);

        /** Push content of array.
            \param T type of elements; one of Segment, IntegerList_t, StringList_t.
            \param array Array of values to push
            \return *this */
        template<typename T>
        Segment& pushBackElements(const T& array);

        /** Push element.
            Ownership for the value will be taken over by the Segment; if an exception is thrown, it is deleted.
            The value will not be cloned.
            \param value Value to push
            \return *this */
        Segment& pushBackNew(Value* value);

        /** Remove last element. Deletes the element.
            \pre size() > 0 */
        void popBack();

        /** Remove last N elements. Deletes the elements.
            \param count Number of elements to delete
            \pre size() >= count */
        void popBackN(Offset_t count);

        /** Get N-th last element.
            \param ofs Number of elements from end to access (0=last, 1=second-last, ...)
            \return value (owned by the Segment), or null
            \pre ofs < size() */
        Value* top(Offset_t ofs) const;

        /** Get last element.
            \return value (owned by the Segment), or null
            \pre size() > 0 */
        Value* top() const;

        /** Extract last element.
            \return Value, ownership will transfer to caller
            \pre size() > 0 */
        Value* extractTop();

        /** Transfer elements from this stack segment to another one.
            The top #count elements are transferred to #other without changing their order.
            \param count Number of elements
            \param other Target segment */
        void transferLastTo(Offset_t count, Segment& other);

     private:
        /*
         *  Generic pushBack function for use by pushBackElements.
         *  These are not part of the public interface because this overload has a nasty ambiguity
         *  for pushBackT(0) (could be int32_t zero or Value*) which is not desirable.
         *  The public interface requires explicit specification of the type.
         */
        void pushBackT(int32_t i)
            { pushBackInteger(i); }
        void pushBackT(const String_t& s)
            { pushBackString(s); }
        void pushBackT(Value* p)
            { pushBack(p); }

        void makeIndexAccessible(Index_t index);

        afl::container::PtrVector<Value> m_content;
    };

} }

template<typename T>
afl::data::Segment&
afl::data::Segment::pushBackElements(const T& array)
{
    for (size_t i = 0, n = array.size(); i < n; ++i) {
        pushBackT(array[i]);
    }
    return *this;
}

#endif
