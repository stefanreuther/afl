/**
  *  \file afl/base/weaklink.hpp
  *  \brief Template class afl::base::WeakLink
  */
#ifndef AFL_AFL_BASE_WEAKLINK_HPP
#define AFL_AFL_BASE_WEAKLINK_HPP

#include "afl/base/baseweaklink.hpp"

namespace afl { namespace base {

    /** Weak link.
        A weak link points at an object.
        If that object is destroyed, the link automatically resets to null.

        Restrictions:
        - WeakTarget/WeakLink do not support multithreading.

        \tparam Target Target type. Must be derived from WeakTarget. */
    template<typename Target>
    class WeakLink : private BaseWeakLink {
     public:
        /** Constructor.
            Makes a null WeakLink. */
        WeakLink()
            : BaseWeakLink(0)
            { }

        /** Constructor.
            Makes a WeakLink pointing at the given object.
            \param p pointer */
        WeakLink(Target* p)
            : BaseWeakLink(p)
            { }

        /** Copy constructor.
            Makes a WeakLink pointing at the same object as the other one.
            \param rhs other WeakLink */
        WeakLink(const WeakLink& rhs)
            : BaseWeakLink(rhs.get())
            { }

        /** Destructor. */
        ~WeakLink()
            { }

        /** Assignment operator.
            Makes this WeakLink point at the same object as the other one.
            \param rhs other WeakLink
            \return *this */
        WeakLink& operator=(const WeakLink& rhs)
            { BaseWeakLink::reset(rhs.get()); return *this; }

        /** Assignment operator.
            Makes this WeakLink point at the given object.
            \param rhs pointer
            \return *this */
        WeakLink& operator=(Target* rhs)
            { BaseWeakLink::reset(rhs); return *this; }

        /** Get contained object.
            \return pointer, can be null */
        Target* get() const
            { return static_cast<Target*>(BaseWeakLink::get()); }

        /** Reset pointer.
            Makes this WeakLink null. */
        void reset()
            { BaseWeakLink::reset(); }
    };

} }

#endif
