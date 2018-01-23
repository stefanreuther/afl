/**
  *  \file afl/base/ref.hpp
  *  \brief Template class afl::base::Ref
  */
#ifndef AFL_AFL_BASE_REF_HPP
#define AFL_AFL_BASE_REF_HPP

#include "afl/base/refcounted.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace base {

    /** Reference-counted non-null pointer.
        This class is similar to Ptr<>, but only allows non-null pointers.
        It tracks the number of active pointers to one object;
        if the last pointer dies, it deletes the object.
        It cannot point to an array.

        Unlike Ptr<>, this one requires the pointed-to type to be a descendant of RefCounted.
        This is because a Ref<> must always be initialized from an object (not a pointer).

        Although this pointer sort-of emulates a reference, we cannot support function calls
        through the "." operator; we still need dereferencing using "->".
        To make it clear that special things happen here, this class has no regular assignment operator.

        Ref<> cooperates with Ptr<>; you can have both Ref<> and Ptr<> point to an object.

        Restrictions:
        - reference-counted pointers do not support circular structures.
        - initialisation must have the form "Ref<T> p(new T)" or "Ref<T> p(otherRef)".

        \param T target type */
    template<typename T>
    class Ref {
        template<typename U> friend class Ref;

     public:
        /** Construct pointer.
            \param p object */
        Ref(T& p);

        /** Copy constructor.
            \param other other smart pointer */
        Ref(const Ref& other);

        /** Converting copy constructor.
            \param U source type (must be derived class of T)
            \param other other smart pointer */
        template<typename U>
        Ref(const Ref<U>& other);

        /** Destructor. */
        ~Ref();

        /** Assign object.
            \param obj other object
            \return *this */
        void reset(T& obj);

        /** Access object.
            \return the object */
        T& get() const;

        /** Dereference operator. */
        T& operator*() const;

        /** Structure access operator. */
        T* operator->() const;

        /** Get smart pointer.
            \return Pointee as a Ptr<> */
        Ptr<T> asPtr() const;

     private:
        // Private assignment operator
        Ref& operator=(const Ref& other);

        /** Embedded pointer. */
        T* m_ptr;

        /** Increment reference counter. */
        void incRef();

        /** Decrement reference counter. */
        void decRef();
    };

} }


template<typename T>
inline
afl::base::Ref<T>::Ref(T& p)
    : m_ptr(&p)
{
    incRef();
}

template<typename T>
inline
afl::base::Ref<T>::Ref(const Ref& other)
    : m_ptr(other.m_ptr)
{
    incRef();
}

template<typename T>
template<typename U>
inline
afl::base::Ref<T>::Ref(const Ref<U>& other)
    : m_ptr(other.m_ptr)
{
    incRef();
}

template<typename T>
inline
afl::base::Ref<T>::~Ref()
{
    decRef();
}

template<typename T>
inline void
afl::base::Ref<T>::reset(T& obj)
{
    // Increment their reference counter
    ++static_cast<RefCounted&>(obj).refCounter();

    // Decrement our reference counter
    decRef();

    // Copy
    m_ptr = &obj;
}

template<typename T>
inline T&
afl::base::Ref<T>::get() const
{
    return *m_ptr;
}

template<typename T>
inline T&
afl::base::Ref<T>::operator*() const
{
    return *m_ptr;
}

template<typename T>
inline T*
afl::base::Ref<T>::operator->() const
{
    return m_ptr;
}

template<typename T>
inline afl::base::Ptr<T>
afl::base::Ref<T>::asPtr() const
{
    return Ptr<T>(m_ptr, &static_cast<RefCounted*>(m_ptr)->refCounter());
}

template<typename T>
inline void
afl::base::Ref<T>::incRef()
{
    ++static_cast<RefCounted*>(m_ptr)->refCounter();
}

template<typename T>
inline void
afl::base::Ref<T>::decRef()
{
    // Note: Ptr<> protects against assignment in the destructor.
    // This cannot be implemented in Ref<>.
    // Whereas Ptr<> just falls back to assigning null, Ref<> cannot do that.
    if (--static_cast<RefCounted*>(m_ptr)->refCounter() == 0) {
        delete m_ptr;
    }
}

#endif
