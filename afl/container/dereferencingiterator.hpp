/**
  *  \file afl/container/dereferencingiterator.hpp
  *  \brief Template class afl::container::DereferencingIterator
  */
#ifndef AFL_AFL_CONTAINER_DEREFERENCINGITERATOR_HPP
#define AFL_AFL_CONTAINER_DEREFERENCINGITERATOR_HPP

#include <iterator>
#include "afl/tmp/dereferencetype.hpp"

namespace afl { namespace container {

    /** Dereferencing iterator.
        If T is an iterator whose value_type is a pointer, i.e. "U*",
        DereferencingIterator<T> iterates over the U's. */
    template<typename T>
    class DereferencingIterator
        : public std::iterator<typename std::iterator_traits<T>::iterator_category,
                               typename afl::tmp::DereferenceType<typename std::iterator_traits<T>::value_type>::Type,
                               typename std::iterator_traits<T>::difference_type,
                               typename std::iterator_traits<T>::pointer,
                               typename std::iterator_traits<T>::reference>
    {
        typedef typename afl::tmp::DereferenceType<typename std::iterator_traits<T>::value_type>::Type Value_t;

        T m_iter;
     public:
        DereferencingIterator()
            : m_iter()
            { }
        DereferencingIterator(T iter)
            : m_iter(iter)
            { }

        bool operator==(const DereferencingIterator& other) const
            { return m_iter == other.m_iter; }
        bool operator!=(const DereferencingIterator& other) const
            { return m_iter != other.m_iter; }
        bool operator<(const DereferencingIterator& other) const
            { return m_iter < other.m_iter; }
        bool operator<=(const DereferencingIterator& other) const
            { return m_iter <= other.m_iter; }
        bool operator>(const DereferencingIterator& other) const
            { return m_iter > other.m_iter; }
        bool operator>=(const DereferencingIterator& other) const
        { return m_iter >= other.m_iter; }

        Value_t& operator*() const
            { return **m_iter; }
        Value_t* operator->() const
            { return *m_iter; }

        DereferencingIterator& operator++()
            { ++m_iter; return *this; }
        DereferencingIterator operator++(int)
            { DereferencingIterator tmp(*this); ++m_iter; return tmp; }
        DereferencingIterator& operator--()
            { --m_iter; return *this; }
        DereferencingIterator operator--(int)
            { DereferencingIterator tmp(*this); --m_iter; return tmp; }

        DereferencingIterator& operator+=(typename std::iterator_traits<T>::difference_type d)
            { m_iter += d; return *this; }
        DereferencingIterator operator+(typename std::iterator_traits<T>::difference_type d)
            { return DereferencingIterator(m_iter + d); }
        DereferencingIterator& operator-=(typename std::iterator_traits<T>::difference_type d)
            { m_iter -= d; return *this; }
        DereferencingIterator operator-(typename std::iterator_traits<T>::difference_type d)
            { return DereferencingIterator(m_iter + d); }
        typename std::iterator_traits<T>::difference_type operator-(const DereferencingIterator& other)
            { return m_iter - other.m_iter; }
        Value_t& operator[](typename std::iterator_traits<T>::difference_type d)
            { return *m_iter[d]; }
        T base()
            { return m_iter; }
    };

} }

#endif
