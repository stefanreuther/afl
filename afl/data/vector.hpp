/**
  *  \file afl/data/vector.hpp
  *  \brief Class afl::data::Vector
  */
#ifndef AFL_AFL_DATA_VECTOR_HPP
#define AFL_AFL_DATA_VECTOR_HPP

#include "afl/base/refcounted.hpp"
#include "afl/base/ptr.hpp"
#include "afl/data/segment.hpp"

namespace afl { namespace data {

    /** Vector. Represents a one-dimensional vector of values.
        Vector objects must always be heap-allocated,
        and are typically referenced from VectorValue instances. */
    class Vector : public afl::base::RefCounted {
     public:
        typedef Segment::Index_t Index_t;

        /** Constructor.
            Makes an empty vector.
            This is a function to make sure all Vector instances are heap-allocated,
            which is required for safe usage in VectorValue. */
        static afl::base::Ptr<Vector> create();

        /** Constructor.
            Makes a populated vector.
            This is a function to make sure all Vector instances are heap-allocated,
            which is required for safe usage in VectorValue.
            \param values Initial values. Will be modified (emptied), Vector takes ownership of contained values. */
        static afl::base::Ptr<Vector> create(Segment& values);

        ~Vector();

        /** Set element by index.
            Ownership for the value remains at caller, even if an exception is thrown.
            The value will be cloned.
            \param index Index to access
            \param value Value to set */
        void set(Index_t index, Value* value);

        /** Set element by index.
            Ownership for the value will be taken over by the Vector; if an exception is thrown, it is deleted.
            The value will not be cloned.
            \param index Index to access
            \param value Value to set */
        void setNew(Index_t index, Value* value);

        /** Get element by index.
            \param index Index to access
            \return value (owned by the Segment), or null */
        Value* get(Index_t index) const;

        /** Get Segment containing all the values. */
        const Segment& getValues() const;

        /** Get Segment containing all the values. */
        Segment& getValues();

        /** Get size.
            \return number of elements in segment */
        Index_t size() const;

        /** Push element.
            Ownership for the value remains at caller, even if an exception is thrown.
            The value will be cloned.
            \param value Value to push */
        void pushBack(Value* value);

        /** Push element.
            Ownership for the value will be taken over by the Segment; if an exception is thrown, it is deleted.
            The value will not be cloned.
            \param value Value to push */
        void pushBackNew(Value* value);

     private:
        /** Constructor.
            Makes an empty vector.
            This constructor is private, use create() to create a heap-allocated Vector. */
        Vector();

        /** Copy constructor. Not implemented.
            (Vector is implicitly uncopyable because the PtrVector contained in the Segment is uncopyable.) */
        Vector(const Vector& other);

        Segment m_values;
    };

} }
#endif
