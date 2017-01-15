/**
  *  \file afl/base/clonable.hpp
  *  \brief Interface afl::base::Clonable
  */
#ifndef AFL_AFL_BASE_CLONABLE_HPP
#define AFL_AFL_BASE_CLONABLE_HPP

namespace afl { namespace base {

    /** Base class for clonable objects.
        Derive class T from Clonable<T> to mark it clonable.
        (This is more a documentation/style issue and does not have magic superpowers.) */
    template<typename T>
    class Clonable {
     public:
        /** Virtual destructor. */
        virtual ~Clonable()
            { }

        /** Clone this object.
            \return newly-allocated, exact copy of this object, never null */
        virtual T* clone() const = 0;

        /** Clone an object.
            Like p->clone(), but allows p to be null.
            \param p Object to clone
            \return newly-allocated clone of object or null */
        static T* cloneOf(const T* p);
    };

} }

template<typename T>
T*
afl::base::Clonable<T>::cloneOf(const T* p)
{
    if (p != 0) {
        return p->clone();
    } else {
        return 0;
    }
}

#endif
