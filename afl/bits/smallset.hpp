/**
  *  \file afl/bits/smallset.hpp
  *  \brief Template class afl::bits::SmallSet
  */
#ifndef AFL_AFL_BITS_SMALLSET_HPP
#define AFL_AFL_BITS_SMALLSET_HPP

#include "afl/base/types.hpp"

namespace afl { namespace bits {

    /** Small set. This encapsulates a type-safe set where members are
        enumerators with numeric values corresponding to the bits of an
        unsigned long (that is, 0..31, usually).

        \param T    Member type
        \param Rep  Underlying type. Must be an integer. */
    template<typename T, typename Rep = uint32_t>
    class SmallSet {
     public:
        /** Underlying type. */
        typedef Rep Rep_t;

        /** Value type. This is the type contained in this set. */
        typedef T Value_t;

        /** Construct empty set. */
        SmallSet()
            : m_rep(0)
            { }

        /** Construct unit set. */
        explicit
        SmallSet(T t)
            : m_rep(Rep_t(1) << t)
            { }

        /** Add an element. */
        SmallSet&
        operator |= (T t)
            { m_rep |= (Rep_t(1) << t); return *this; }

        /** Add a set. */
        SmallSet&
        operator |= (SmallSet other)
            { m_rep |= other.m_rep; return *this; }

        /** Set union (set plus single element). */
        SmallSet
        operator | (T t) const
            { return SmallSet<T>(*this) |= t; }

        /** Set union. */
        SmallSet
        operator | (SmallSet other) const
            { return SmallSet<T>(*this) |= other; }

        /** Add an element (alternate name). */
        SmallSet&
        operator += (T t)
            { m_rep |= (Rep_t(1) << t); return *this; }

        /** Add a set (alternate name). */
        SmallSet&
        operator += (SmallSet other)
            { m_rep |= other.m_rep; return *this; }

        /** Set union (set plus single element, alternate name). */
        SmallSet
        operator + (T t) const
            { return SmallSet<T>(*this) |= t; }

        /** Set union (alternate name). */
        SmallSet
        operator + (SmallSet other) const
            { return SmallSet<T>(*this) |= other; }

        /** Remove an element. */
        SmallSet&
        operator -= (T t)
            { m_rep &= ~(Rep_t(1) << t); return *this; }

        /** Remove a set. */
        SmallSet&
        operator -= (SmallSet other)
            { m_rep &= ~other.m_rep; return *this; }

        /** Set difference (set minus single element). */
        SmallSet
        operator - (T t) const
            { return SmallSet<T>(*this) -= t; }

        /** Set difference. */
        SmallSet
        operator - (SmallSet other) const
            { return SmallSet<T>(*this) -= other; }

        /** Conjunction (intersection). */
        SmallSet&
        operator &= (SmallSet other)
            { m_rep &= other.m_rep; return *this; }

        /** Conjuction (intersection). */
        SmallSet
        operator & (SmallSet other) const
            { return SmallSet<T>(*this) &= other; }

        /** Toggle an element (symmetric difference). */
        SmallSet&
        operator ^= (T t)
            { m_rep ^= (Rep_t(1) << t); return *this; }

        /** Toggle elements from other set (symmetric difference). */
        SmallSet&
        operator ^= (SmallSet other)
            { m_rep ^= other.m_rep; return *this; }

        /** Symmetric set difference. */
        SmallSet
        operator ^ (T t) const
            { return SmallSet<T>(*this) ^= t; }

        /** Symmetric set difference. */
        SmallSet
        operator ^ (SmallSet other) const
            { return SmallSet<T>(*this) ^= other; }

        /** Test whether element is in. */
        bool
        contains(T t) const
            { return (m_rep & (Rep_t(1) << t)) != 0; }

        /** Test whether this set contains all elements from the other one. */
        bool
        contains(SmallSet other) const
            { return (other.m_rep & ~m_rep) == 0; }

        /** Test whether this set contains any of the elements from the
            other one. */
        bool
        containsAnyOf(SmallSet other) const
            { return (m_rep & other.m_rep) != 0; }

        /** Test whether this set is empty. */
        bool
        empty() const
            { return m_rep == 0; }

        /** Clear this set. \post empty() */
        void
        clear()
            { m_rep = 0; }

        /** Check whether this set contains at most one element. */
        bool
        isUnitSet() const
            { return (m_rep & (m_rep-1)) == 0; }

        /** Static constructor: construct a set which contains all objects
            from T(0) up to and including T(t) */
        static SmallSet
        allUpTo(T t)
            {
                // This wording allows constructing all values without overflow:
                //   1 << t       get one 1 bit and 't' 0 bits
                //   - 1          get 't' 1 bits
                //   << 1         get 't' 1 bits and 1 0 bit
                //   + 1          get 't+1' 1 bits
                Rep_t value = (((Rep_t(1) << t) - 1) << 1) + 1;
                return SmallSet<T>(value, true);
            }

        /** Compare for equality. */
        bool
        operator==(SmallSet other) const
            {
                return m_rep == other.m_rep;
            }

        /** Compare for equality to unit set. */
        bool
        operator==(T t) const
            {
                return m_rep == Rep_t(1) << t;
            }

        /** Compare for inequality. */
        bool
        operator!=(SmallSet other) const
            {
                return m_rep != other.m_rep;
            }

        /** Compare for inequality to unit set. */
        bool
        operator!=(T t) const
            {
                return m_rep != Rep_t(1) << t;
            }

        /** True iff set is non-empty. Shortcut for !empty(). */
        bool
        nonempty() const
            {
                return m_rep != 0;
            }

        /** Get representation. Returns an integer where
            a.contains(x) iff (a.toInteger() & (1 << x)) != 0 */
        Rep_t toInteger() const
            { return m_rep; }

        /** Create set from integer. Each set bit in \c v translates into
            a set element. */
        static SmallSet fromInteger(Rep_t v)
            { return SmallSet<T>(v, true); }

     private:
        Rep_t m_rep;

        SmallSet(Rep_t r, bool)
            : m_rep(r)
            { }
    };
} }

#endif
