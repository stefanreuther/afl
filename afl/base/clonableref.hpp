/**
  *  \file afl/base/clonableref.hpp
  *  \brief Template class afl::base::ClonableRef
  */
#ifndef AFL_AFL_BASE_CLONABLEREF_HPP
#define AFL_AFL_BASE_CLONABLEREF_HPP

namespace afl { namespace base {

    /** A clonable, non-nullable, mutable reference.
        Behaves like a pointer to a T.
        When the pointer is copied, the object T is copied by using its 'clone' method.
        ClonableRef is not intended to hold null pointers.

        \param T Type. The type must have a clone() method. */
    template<typename T>
    class ClonableRef {
     public:
        /** Constructor.
            \param t Initial value. Must be obtained from 'new'; ideally directly in the initialisation. Must not be null. */
        ClonableRef(T* t)
            : m_ref(t)
            { }

        /** Copy constructor.
            \param other Other reference. Its object is cloned. */
        ClonableRef(const ClonableRef& other)
            : m_ref(other.m_ref->clone())
            { }

        /** Destructor. */
        ~ClonableRef()
            { delete m_ref; }

        /** Assignment operator. Discards this ClonableRef's object and initializes with a clone of other's.
            \param other Other reference.
            \return *this */
        ClonableRef& operator=(ClonableRef& other)
            {
                if (other.m_ref != m_ref) {
                    T* newRef = other.m_ref->clone();
                    delete m_ref;
                    m_ref = newRef;
                }
                return *this;
            }

        /** Dereference as structure. */
        T* operator->() const
            { return m_ref; }

        /** Dereference. */
        T& operator*() const
            { return *m_ref; }

     private:
        /** Underlying pointer. */
        T* m_ref;
    };

} }

#endif
