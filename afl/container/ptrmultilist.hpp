/**
  *  \file afl/container/ptrmultilist.hpp
  *  \brief Template class afl::container::PtrMultiList
  */
#ifndef AFL_AFL_CONTAINER_PTRMULTILIST_HPP
#define AFL_AFL_CONTAINER_PTRMULTILIST_HPP

#include <functional>                  // std::less
#include <algorithm>                   // std::swap
#include "afl/base/types.hpp"
#include "afl/container/ptrmultilistbase.hpp"
#include "afl/base/uncopyable.hpp"

namespace afl { namespace container {

    /** List of pointers with multiple iterators.
        Implements ownership semantics for a list of pointers.
        In addition, allows multiple iterators into it,
        where deletion and/or insertion invalidates neither.

        If one iterator is used to delete an element, other iterators
        pointing at that element will return null.

        Iterators are forward iterators.

        This list allows append at the end, but removal only at the
        beginning. */
    template<typename T>
    class PtrMultiList : public afl::base::Uncopyable {
     public:
        /** Iterator. */
        class iterator {
            friend class PtrMultiList;
         public:
            iterator(PtrMultiListBase::iterator it)
                : m_it(it)
                { }
            iterator()
                : m_it()
                { }
            T* operator*() const
                { return static_cast<T*>(m_it.get()); }
            iterator& operator++()
                { m_it.next(); return *this; }
            iterator operator++(int)
                { iterator tmp(m_it); m_it.next(); return tmp; }
            bool operator==(const iterator& other) const
                { return m_it == other.m_it; }
            bool operator!=(const iterator& other) const
                { return m_it != other.m_it; }
         private:
            PtrMultiListBase::iterator m_it;
        };

        typedef T* value_type;
        typedef iterator const_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        /** Constructor. Makes a blank list. */
        PtrMultiList()
            : m_data(deleter)
            { }

        /** Destructor. Clears the list and deletes all elements. */
        ~PtrMultiList()
            { }

        /** Clear the list. Deletes all elements and invalidatas all iterators. */
        void clear()
            { m_data.clear(); }

        /** Check emptiness. */
        bool empty() const
            { return m_data.empty(); }

        /** Append a newly-allocated element. If the operation fails, the element
            will be deleted before the exception is rethrown. */
        T* pushBackNew(T* t)
            {
                m_data.pushBackNew(t);
                return t;
            }

        /** Prepend a newly-allocated element. If the operation fails, the element
            will be deleted before the exception is rethrown. */
        T* pushFrontNew(T* t)
            {
                m_data.pushFrontNew(t);
                return t;
            }

        /** Remove and delete first element. */
        void popFront()
            { m_data.erase(m_data.begin()); }

        /** Erase an element. */
        void erase(const iterator& it)
            { m_data.erase(it.m_it); }

        /** Insert an element.
            \param it element is inserted before this item
            \param p element
            \return iterator pointing to newly-inserted element */
        iterator insertNew(const iterator& it, T* p)
            { return m_data.insert(it.m_it, p); }

        /** Swap elements. Both iterators must point at undeleted elements. */
        void swapElements(const iterator& a, const iterator& b)
            { m_data.swapElements(a.m_it, b.m_it); }

        /** Extract an element. Removes it from the sequence and returns its value.
            Caller assumes responsibility. */
        T* extractElement(iterator it)
            { return static_cast<T*>(m_data.extractElement(it.m_it)); }

        /** Get iterator to first element. */
        iterator begin() const
            { return m_data.begin(); }

        /** Get iterator to one-past-last element. */
        iterator end() const
            { return m_data.end(); }

        /** Get first element. */
        T* front() const
            { return static_cast<T*>(m_data.front()); }

        /** Get first element and remove it from the sequence.
            Caller assumes responsibility. */
        T* extractFront()
            { return static_cast<T*>(m_data.extractFront()); }

        /** Get size of list (number of elements). */
        size_type size() const
            { return m_data.size(); }

        /** Sort. */
        void sort()
            { sort(std::less<T*>()); }

        template<typename Pred>
        void sort(Pred p);

        /** Merge. */
        void merge(PtrMultiList& other)
            { merge(other, std::less<T*>()); }

        template<typename Pred>
        void merge(PtrMultiList& other, Pred p);

     private:
        static void deleter(void* p)
            { delete static_cast<T*>(p); }
        PtrMultiListBase m_data;
    };

} } // afl::container

/******************************** Inlines ********************************/

/** Sort with predicate. A simple merge sort.
    All iterators are invalidated.
    \param p Predicate */
template<typename T>
template<typename Pred>
void
afl::container::PtrMultiList<T>::sort(Pred p)
{
    // Move every other element into another list
    PtrMultiList<T> tmp;
    size_type n = 0;
    for (iterator i = begin(), e = end(); i != e; ++i, ++n) {
        if ((n & 1) != 0) {
            tmp.pushBackNew(extractElement(i));
        }
    }

    // Sort sub-lists
    if (n > 2) {
        tmp.sort(p);
        sort(p);
    }

    // Merge
    merge(tmp, p);
}

/** Merge with predicate. Assumes this list as well as the other list to be sorted,
    and merges the two. %other will be empty afterwards. All iterators are invalidated.
    \param p Predicate
    \param other Other list */
template<typename T>
template<typename Pred>
void
afl::container::PtrMultiList<T>::merge(PtrMultiList& other, Pred p)
{
    iterator i = begin(), e = end();
    while (!other.empty()) {
        T* elem = other.extractFront();
        while (i != e && p(*i, elem)) {
            ++i;
        }
        insertNew(i, elem);
    }
}

#endif
