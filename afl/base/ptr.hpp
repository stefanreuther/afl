/**
  *  \file afl/base/ptr.hpp
  *  \brief Template class afl::base::Ptr
  */
#ifndef AFL_AFL_BASE_PTR_HPP
#define AFL_AFL_BASE_PTR_HPP

#include "afl/base/refcounted.hpp"
#include "afl/sys/atomicinteger.hpp"

namespace afl { namespace base {

    namespace detail {
        afl::sys::AtomicInteger* getReferenceCount(const void*);
        afl::sys::AtomicInteger* getReferenceCount(const afl::base::RefCounted*);
    }

    template<typename T> class Ref;

    /** Reference-counted pointer.
        This pointer supports all operations which a dumb pointer-to-a-single-object would support.
        It tracks the number of active pointers to one object;
        if the last pointer dies, it deletes the object.
        It cannot point to an array.

        It is possible to pass a smart pointer "through" a dumb one.
        This is most efficient if the pointed-to type is derived from RefCounted.

        Restrictions:
        - reference-counted pointers do not support circular structures.
        - allocated reference count objects are never freed.
          This restriction is lifted if objects are derived from RefCounted.
        - passing smart pointers "through" dumb ones only works if all pointers
          actually are the same; in a class hierarchy, that is, for a Derived* d,
          (void*)(Base*) d == (void*) d.
          Although not required by ISO C++, this is true in all compilers known to me
          (GNU, Borland, MS, ADI) for single inheritance,
          and never true for multiple inheritance.
          Therefore, if Ptr is used for multiple-inheritance,
          the pointers must not leave the smart pointer universe.

        \param T target type */
    template<typename T>
    class Ptr {
        template<typename U> friend class Ptr;
        friend class Ref<T>;

     public:
        /** Construct null pointer. */
        Ptr();

        /** Construct pointer.
            \param p dumb pointer (can be null) */
        Ptr(T* p);

        /** Copy constructor.
            \param other other smart pointer */
        Ptr(const Ptr& other);

        /** Converting copy constructor.
            \param U source type (must be derived class of T)
            \param other other smart pointer */
        template<typename U>
        Ptr(const Ptr<U>& other);

        /** Destructor. */
        ~Ptr();

        /** Assignment operator.
            \param other other smart pointer
            \return *this */
        Ptr& operator=(const Ptr& other);

        /** Converting assignment operator.
            \param U source type (must be derived class of T)
            \param other other smart pointer
            \return *this */
        template<typename U>
        Ptr& operator=(const Ptr<U>& other);

        /** Assign dumb pointer.
            \param p dumb pointer (can be null)
            \return *this */
        Ptr& operator=(T* p);

        /** Extract dumb pointer.
            \return the dumb pointer (can be null) */
        T* get() const;

        /** Reset this pointer (makes it null).
            \post get() == 0 */
        void reset();

        /** Dereference operator. */
        T& operator*() const;

        /** Structure access operator. */
        T* operator->() const;

        /** Smart dynamic_cast.
            \param U target type
            \return smart pointer referring to the same object as this one if the cast was successfull,
            smart null pointer otherwise */
        template<typename U>
        Ptr<U> cast() const;

        /** Compare smart pointer for equality.
            \param other Other pointer
            \return true iff equal */
        template<typename U>
        bool operator==(const Ptr<U>& other) const;

        /** Compare pointer for equality.
            \param other Other pointer
            \return true iff equal */
        template<typename U>
        bool operator==(const U* other) const;

        /** Compare smart pointer for inequality.
            \param other Other pointer
            \return true iff different */
        template<typename U>
        bool operator!=(const Ptr<U>& other) const;

        /** Compare pointer for inequality.
            \param other Other pointer
            \return true iff different */
        template<typename U>
        bool operator!=(const U* other) const;

     private:
        /** Embedded pointer. */
        T* m_ptr;

        /** Reference counter. Null iff m_ptr is null. */
        afl::sys::AtomicInteger* m_ref;

        /** Private constructor. Construct from a pointer and reference count explicitly given. */
        Ptr(T* ptr, afl::sys::AtomicInteger* ref);

        /** Increment reference counter. */
        void incRef();

        /** Decrement reference counter. */
        void decRef();

        /** Common implementation of assignment operators. */
        void assign(T* ptr, afl::sys::AtomicInteger* ref);
    };

} }

inline afl::sys::AtomicInteger*
afl::base::detail::getReferenceCount(const afl::base::RefCounted* p)
{
    return &p->refCounter();
}


template<typename T>
inline
afl::base::Ptr<T>::Ptr()
    : m_ptr(0),
      m_ref(0)
{ }

template<typename T>
inline
afl::base::Ptr<T>::Ptr(T* p)
    : m_ptr(p),
      m_ref(0)
{
    if (p) {
        m_ref = detail::getReferenceCount(p);
        ++*m_ref;
    }
}

template<typename T>
inline
afl::base::Ptr<T>::Ptr(const Ptr& other)
    : m_ptr(other.m_ptr),
      m_ref(other.m_ref)
{
    if (m_ref) {
        incRef();
    }
}

template<typename T>
template<typename U>
inline
afl::base::Ptr<T>::Ptr(const Ptr<U>& other)
    : m_ptr(other.m_ptr),
      m_ref(other.m_ref)
{
    if (m_ref) {
        incRef();
    }
}

template<typename T>
inline
afl::base::Ptr<T>::~Ptr()
{
    // decRef calls the user-defined destructor, which may modify the pointer again.
    // Hence, loop until we're really empty. See the testAssignDestructor test.
    while (m_ref != 0) {
        decRef();
    }
}


template<typename T>
inline afl::base::Ptr<T>&
afl::base::Ptr<T>::operator=(const Ptr& other)
{
    assign(other.m_ptr, other.m_ref);
    return *this;
}

template<typename T>
template<typename U>
inline afl::base::Ptr<T>&
afl::base::Ptr<T>::operator=(const Ptr<U>& other)
{
    assign(other.m_ptr, other.m_ref);
    return *this;
}

template<typename T>
inline afl::base::Ptr<T>&
afl::base::Ptr<T>::operator=(T* p)
{
    assign(p, p ? detail::getReferenceCount(p) : 0);
    return *this;
}

template<typename T>
inline T*
afl::base::Ptr<T>::get() const
{
    return m_ptr;
}

template<typename T>
inline void
afl::base::Ptr<T>::reset()
{
    decRef();
}

template<typename T>
inline T&
afl::base::Ptr<T>::operator*() const
{
    return *m_ptr;
}

template<typename T>
inline T*
afl::base::Ptr<T>::operator->() const
{
    return m_ptr;
}

template<typename T>
template<typename U>
inline afl::base::Ptr<U>
afl::base::Ptr<T>::cast() const
{
    if (U* p = dynamic_cast<U*>(m_ptr)) {
        return Ptr<U>(p, m_ref);
    } else {
        return Ptr<U>();
    }
}

template<typename T>
template<typename U>
inline bool
afl::base::Ptr<T>::operator==(const Ptr<U>& other) const
{
    return m_ptr == other.m_ptr;
}

template<typename T>
template<typename U>
inline
bool
afl::base::Ptr<T>::operator==(const U* other) const
{
    return m_ptr == other;
}

template<typename T>
template<typename U>
inline bool
afl::base::Ptr<T>::operator!=(const Ptr<U>& other) const
{
    return m_ptr != other.m_ptr;
}

template<typename T>
template<typename U>
inline
bool
afl::base::Ptr<T>::operator!=(const U* other) const
{
    return m_ptr != other;
}

template<typename T>
inline
afl::base::Ptr<T>::Ptr(T* ptr, afl::sys::AtomicInteger* ref)
    : m_ptr(ptr),
      m_ref(ref)
{
    if (ref) {
        ++*ref;
    }
}

template<typename T>
inline void
afl::base::Ptr<T>::incRef()
{
    if (m_ref) {
        ++*m_ref;
    }
}

template<typename T>
inline void
afl::base::Ptr<T>::decRef()
{
    // Save the Ptr instance variables locally and reset the state.
    // This way, a possible user-defined destructor sees a consistent state.
    T* p = m_ptr;
    afl::sys::AtomicInteger* ref = m_ref;
    m_ptr = 0;
    m_ref = 0;

    // Destructor work
    if (ref) {
        if (--*ref == 0) {
            delete p;
        }
    }
}

template<typename T>
inline void
afl::base::Ptr<T>::assign(T* ptr, afl::sys::AtomicInteger* ref)
{
    if (ref) {
        ++*ref;
    }
    decRef();
    m_ptr = ptr;
    m_ref = ref;
}

#endif
