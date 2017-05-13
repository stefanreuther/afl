/**
  *  \file afl/data/vector.hpp
  *  \brief Class afl::data::Vector
  */
#ifndef AFL_AFL_DATA_VECTOR_HPP
#define AFL_AFL_DATA_VECTOR_HPP

#include "afl/base/refcounted.hpp"
#include "afl/base/ref.hpp"
#include "afl/data/segment.hpp"

namespace afl { namespace data {

    /** Vector. Represents a one-dimensional vector of values.
        This is a wrapper around Segment that makes sure that all Vector objects are heap-allocated.
        Vector objects are typically referenced from VectorValue instances. */
    class Vector : public afl::base::RefCounted, public Segment {
     public:
        /** Reference type. */
        typedef afl::base::Ref<Vector> Ref_t;

        /** Constructor.
            Makes an empty vector.
            This is a function to make sure all Vector instances are heap-allocated,
            which is required for safe usage in VectorValue. */
        static Ref_t create();

        /** Constructor.
            Makes a populated vector.
            This is a function to make sure all Vector instances are heap-allocated,
            which is required for safe usage in VectorValue.
            \param values Initial values. Will be modified (emptied), Vector takes ownership of contained values. */
        static Ref_t create(Segment& values);

        ~Vector();

     private:
        /** Constructor.
            Makes an empty vector.
            This constructor is private, use create() to create a heap-allocated Vector. */
        Vector();

        /** Copy constructor. Not implemented.
            (Vector is implicitly uncopyable because the PtrVector contained in the Segment is uncopyable.) */
        Vector(const Vector& other);
    };

} }
#endif
