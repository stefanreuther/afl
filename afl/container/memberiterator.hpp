/**
  *  \file afl/container/memberiterator.hpp
  *  \brief Template class afl::container::MemberIterator
  */
#ifndef AFL_AFL_CONTAINER_MEMBERITERATOR_HPP
#define AFL_AFL_CONTAINER_MEMBERITERATOR_HPP

#include <iterator>

namespace afl { namespace container {

    /** Member iterator. If T is an iterator whose value_type is a structure,
        this iterates over a particular member of all these structures. For
        example, <tt>member_iterator<std::map<int,int>::iterator,int,&std::pair<const int,int>::second></tt>
        iterates over all values in the map.
        \param T   Iterator type
        \param M   Member type
        \param Mem Member */
    template<typename T, typename M, M (T::value_type::*Mem)>
    class MemberIterator
        : public std::iterator<typename std::iterator_traits<T>::iterator_category,
                               M,
                               typename std::iterator_traits<T>::difference_type,
                               typename std::iterator_traits<T>::pointer,
                               typename std::iterator_traits<T>::reference>
    {
        typedef M Value_t;
        T m_iter;
     public:
        MemberIterator()
            : m_iter()
            { }
        MemberIterator(T iter)
            : m_iter(iter)
            { }

        bool operator==(const MemberIterator& other) const
            { return m_iter == other.m_iter; }
        bool operator!=(const MemberIterator& other) const
            { return m_iter != other.m_iter; }
        bool operator<(const MemberIterator& other) const
            { return m_iter < other.m_iter; }
        bool operator<=(const MemberIterator& other) const
            { return m_iter <= other.m_iter; }
        bool operator>(const MemberIterator& other) const
            { return m_iter > other.m_iter; }
        bool operator>=(const MemberIterator& other) const
            { return m_iter >= other.m_iter; }

        Value_t& operator*() const
            { return (*m_iter).*Mem; }
        Value_t* operator->() const
            { return &(*m_iter).*Mem; }

        MemberIterator& operator++()
            { ++m_iter; return *this; }
        MemberIterator operator++(int)
            { MemberIterator tmp(*this); ++m_iter; return tmp; }
        MemberIterator& operator--()
            { --m_iter; return *this; }
        MemberIterator operator--(int)
            { MemberIterator tmp(*this); --m_iter; return tmp; }

        MemberIterator& operator+=(typename std::iterator_traits<T>::difference_type d)
            { m_iter += d; return *this; }
        MemberIterator operator+(typename std::iterator_traits<T>::difference_type d)
            { return MemberIterator(m_iter + d); }
        MemberIterator& operator-=(typename std::iterator_traits<T>::difference_type d)
            { m_iter -= d; return *this; }
        MemberIterator operator-(typename std::iterator_traits<T>::difference_type d)
            { return MemberIterator(m_iter + d); }
        typename std::iterator_traits<T>::difference_type operator-(const MemberIterator& other)
            { return m_iter - other.m_iter; }
        Value_t& operator[](typename std::iterator_traits<T>::difference_type d)
            { return (*m_iter[d]).*Mem; }
        T base()
            { return m_iter; }
    };

} }

#endif
