/**
  *  \file afl/base/refcounted.hpp
  *  \brief Base class afl::base::RefCounted
  */
#ifndef AFL_AFL_BASE_REFCOUNTED_HPP
#define AFL_AFL_BASE_REFCOUNTED_HPP

#include "afl/sys/atomicinteger.hpp"

namespace afl { namespace base {

    /** Reference-counted object base class.
        Deriving from this class adds a reference counter to the object.
        This optimizes operation when the object is used with Ptr<>,
        and could also be usable otherwise.

        RefCounted does not add a virtual destructor to the class,
        and is therefore not intended to be used with <tt>delete</tt>. */
    class RefCounted {
     public:
        /** Access reference counter.
            Use this to actually manage the reference count.
            \return reference counter */
        afl::sys::AtomicInteger& refCounter() const;

     protected:
        /** Constructor. Makes a new un-referenced object. */
        RefCounted();

        /** Copy constructor. Makes a new un-referenced object.
            Does NOT copy the other object's reference count.
            This constructor exists so that a derived class's auto-generated copy constructor does the right thing.
            \param other object to copy from */
        RefCounted(const RefCounted& other);

        /** Destructor. Must be protected to avoid anyone attempting to delete a descendant
            through a RefCounted pointer. */
        ~RefCounted();

        /** Assignment operator.
            Does NOT copy the other object's reference count.
            This operator exists so that a derived class's auto-generated operator does the right thing.
            \param other object to copy from
            \return *this */
        RefCounted& operator=(const RefCounted& other);

     private:
        mutable afl::sys::AtomicInteger m_refCounter;
    };

} }

inline afl::sys::AtomicInteger&
afl::base::RefCounted::refCounter() const
{
    return m_refCounter;
}

inline
afl::base::RefCounted::RefCounted()
    : m_refCounter(0)
{ }

inline
afl::base::RefCounted::RefCounted(const RefCounted& /*other*/)
    : m_refCounter(0)
{ }

inline
afl::base::RefCounted::~RefCounted()
{ }

inline afl::base::RefCounted&
afl::base::RefCounted::operator=(const RefCounted& /*other*/)
{
    return *this;
}

#endif
