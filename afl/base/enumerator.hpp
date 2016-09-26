/**
  *  \file afl/base/enumerator.hpp
  *  \brief Interface afl::base::Enumerator
  */
#ifndef AFL_AFL_BASE_ENUMERATOR_HPP
#define AFL_AFL_BASE_ENUMERATOR_HPP

#include "afl/base/deletable.hpp"

namespace afl { namespace base {

    /** Enumerator. A generalized form of an input iterator, differing
        from the STL iterator mainly in the fact that this one explicitly
        provides late binding and does not require the iterator to be
        copyable. */
    template<typename T>
    class Enumerator : public Deletable {
     public:
        /** Virtual destructor. */
        virtual ~Enumerator();

        /** Get next element
            \param result [out] The element
            \retval false No more elements; result has unspecified content
            \retval true New element has been produced */
        virtual bool getNextElement(T& result) = 0;
    };
} }

template<typename T>
inline
afl::base::Enumerator<T>::~Enumerator()
{ }

#endif
