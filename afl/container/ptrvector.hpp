/**
  *  \file afl/container/ptrvector.hpp
  *  \brief Template class afl::container::PtrVector
  */
#ifndef AFL_AFL_CONTAINER_PTRVECTOR_HPP
#define AFL_AFL_CONTAINER_PTRVECTOR_HPP

#include <vector>
#include <utility>
#include <iterator>
#include <cassert>
#include <algorithm>
#include "afl/base/uncopyable.hpp"
#include "afl/tmp/copycv.hpp"

namespace afl { namespace container {

    /** Vector of Pointers. Implements ownership semantics for a vector of
        pointers, and attempts to manage it in a type- and exception-safe way.

        The basic assumption is that PtrVector<> owns objects passed to it.
        It will delete them when the slot holding the pointer goes away. You
        must not delete a pointer obtained from PtrVector<>, or place it in
        another PtrVector<>.

        Otherwise, the interface attempts to be somewhat close to
        std::vector<>, restricted to operations that are safe enough to
        maintain ownership semantics. */
    template<typename T>
    class PtrVector : private afl::base::Uncopyable {
     public:
        class iterator;
        typedef iterator const_iterator;
        typedef typename std::vector<void*>::size_type size_type;
        typedef typename std::vector<void*>::difference_type difference_type;
        typedef T* value_type;

        /** Default constructor.
            Makes an empty vector. */
        PtrVector();

        /** Destructor.
            Deletes all objects contained in the vector. */
        ~PtrVector();

        /** Append new element.
            Takes ownership of the element.
            If appending fails (out-of-memory condition), this function deletes \c t.
            \param t newly-allocated element to append, or null
            \return t */
        T* pushBackNew(T* t);

        /** Insert element.
            The element is inserted before the given iterator.
            Takes ownership of the element.
            If inserting fails (out-of-memory condition), this function deletes \c t.
            \param it position (use begin() to prepend, end() to append)
            \param t newly-allocated element to insert, or null
            \return t */
        T* insertNew(const iterator& it, T* t);

        /** Drop last element.
            Deletes and removes the element.
            \pre !empty() */
        void popBack();

        /** Swap two vectors.
            \param other Vector to swap with */
        void swap(PtrVector& other);

        /** Get size of this vector.
            \return size */
        size_type size() const;

        /** Check emptiness.
            \return true if vector is empty (size()=0). */
        bool empty() const;

        /** Clear this vector.
            Deletes all elements. */
        void clear();

        /** Index access.
            \param index element index, [0,size())
            \return element */
        T* operator[](size_type index) const;

        /** Get first object.
            \pre !empty()
            \return element */
        T* front() const;

        /** Get last object.
            \pre !empty()
            \return element */
        T* back() const;

        /** Get begin iterator.
            \return iterator */
        const_iterator begin() const;

        /** Get end iterator.
            \return iterator */
        const_iterator end() const;

        /** Reserve space.
            Make sure we can store at least \c size objects in this vector without re-allocation.
            \param size desired size */
        void reserve(size_type size);

        /** Erase single element.
            \param i Iterator pointing to element. The element will be deleted. */
        void erase(iterator i);

        /** Erase range.
            \param i,j Range [i,j). Elements will be deleted and removed from the vector. */
        void erase(iterator i, iterator j);

        /** Sort.
            Null elements are sorted at the end.
            Non-null elements are sorted by comparing their content using std::less<T>. */
        void sort();

        /** Sort by predicate.
            Null elements are sorted at the end.
            Non-null elements are sorted by comparing their content using p::operator()(T,T).
            \param Predicate predicate type
            \param p Predicate instance */
        template<typename Predicate>
        void sort(Predicate p);

        /*
         *  PtrVector-specific functions:
         */

        /** Swap elements.
            \param a first index, [0,size())
            \param b second index, [0,size()) */
        void swapElements(size_type a, size_type b);

        /** Swap elements of different PtrVector's.
            \param a first index, pointing into this vector [0,size())
            \param other other vector (can be the same as *this)
            \param b other index, pointing into other vector [0,other.size()) */
        void swapElements(size_type a, PtrVector& other, size_type b);

        /** Replace an element.
            Takes ownership of the element.
            The replaced element will be deleted.
            \param a index [0,size())
            \param value value to store there */
        void replaceElementNew(size_type a, T* value);

        /** Destructively extract element from this vector.
            The element will be replaced by 0, the old value will be returned.
            The caller assumes responsibility for freeing it.
            \param a index [0,size())
            \return extracted element */
        T* extractElement(size_type a);

        /** Destructively extract last element from this vector.
            The element will be removed from the vector, reducing the vector's size.
            The caller assumes responsibility for freeing it.
            \return extracted element */
        T* extractLast();

     private:
        /** Content. Implemented as vector<void*>, so all instantiations share
            their delete, resize, etc. implementations. */
        std::vector<void*> m_data;
        void deleteAll();

        class StandardPredicate;

        template<typename Predicate>
        class TemplatePredicate;
    };

    /** Random-access iterator. Supports
        - post/pre increment/decrement
        - addition and subtraction of difference_type
        - subtraction of another iterator
        - indexing
        - dereferencing
        - comparison */
    template<typename T>
    class PtrVector<T>::iterator
        : public std::iterator<std::random_access_iterator_tag,
                               const T*,
                               std::vector<void*>::difference_type,
                               const T*,
                               const T*&>
    {
        std::vector<void*>::const_iterator m_it;
     public:
        typedef std::vector<void*>::difference_type difference_type;

        // Constructor
        iterator(std::vector<void*>::const_iterator it)
            : m_it(it) { }

        // Increment and Decrement
        iterator& operator++()
            { ++m_it; return *this; }
        iterator operator++(int)
            { iterator res(m_it); ++m_it; return res; }
        iterator& operator--()
            { --m_it; return *this; }
        iterator operator--(int)
            { iterator res(m_it); --m_it; return res; }

        // Add and subtract
        iterator operator+(difference_type n) const
            { return iterator(m_it + n); }
        iterator operator-(difference_type n) const
            { return iterator(m_it - n); }

        // Advance
        iterator& operator+=(difference_type n)
            { m_it += n; return *this; }
        iterator& operator-=(difference_type n)
            { m_it -= n; return *this; }

        // Distance
        difference_type operator-(iterator rhs) const
            { return m_it - rhs.m_it; }

        // Comparison
        bool operator<(iterator rhs) const
            { return m_it < rhs.m_it; }
        bool operator>(iterator rhs) const
            { return m_it > rhs.m_it; }
        bool operator<=(iterator rhs) const
            { return m_it <= rhs.m_it; }
        bool operator>=(iterator rhs) const
            { return m_it >= rhs.m_it; }
        bool operator==(iterator rhs) const
            { return m_it == rhs.m_it; }
        bool operator!=(iterator rhs) const
            { return m_it != rhs.m_it; }

        // Dereferencing and indexing
        T* operator*() const
            { return static_cast<T*>(*m_it); }
        T* operator[](int n) const
            { return static_cast<T*>(m_it[n]); }
    };

} }

/***************************** Implementation ****************************/

template<typename T>
class afl::container::PtrVector<T>::StandardPredicate {
 public:
    bool operator()(const void* a, const void* b)
        {
            if (a == 0 || b == 0) {
                return std::less<const void*>()(b, a);
            } else {
                return std::less<T>()(*static_cast<const T*>(a), *static_cast<const T*>(b));
            }
        }
};

template<typename T>
template<typename Predicate>
class afl::container::PtrVector<T>::TemplatePredicate {
 public:
    TemplatePredicate(Predicate& pred)
        : m_predicate(pred)
        { }
    bool operator()(const void* a, const void* b)
        {
            if (a == 0 || b == 0) {
                return std::less<const void*>()(b, a);
            } else {
                return m_predicate(*static_cast<const T*>(a), *static_cast<const T*>(b));
            }
        }
 private:
    Predicate& m_predicate;
};


// Default constructor.
template<typename T>
inline
afl::container::PtrVector<T>::PtrVector()
    : Uncopyable(),
      m_data()
{ }

// Destructor.
template<typename T>
afl::container::PtrVector<T>::~PtrVector()
{
    deleteAll();
}

// Append new element.
template<typename T>
T*
afl::container::PtrVector<T>::pushBackNew(T* t)
{
    typename afl::tmp::CopyCV<T, void>::Type* tv = t;
    try {
        m_data.push_back(const_cast<void*>(tv));
        return t;
    }
    catch (...) {
        delete t;
        throw;
    }
}

// Insert element.
template<typename T>
T*
afl::container::PtrVector<T>::insertNew(const iterator& it, T* t)
{
    // This is miscompiled by g++-3.4.4 (cygming special) if 'it' is passed by copy.
    // (The compiler seems to forget adding m_data.begin() to the bit pattern passed to
    // m_data.insert() as iterator.)
    typename afl::tmp::CopyCV<T, void>::Type* tv = t;
    try {
        difference_type index = it - begin();
        m_data.insert(m_data.begin() + index, const_cast<void*>(tv));
        return t;
    }
    catch (...) {
        delete t;
        throw;
    }
}

// Drop last element.
template<typename T>
void
afl::container::PtrVector<T>::popBack()
{
    assert(!m_data.empty());
    delete static_cast<T*>(m_data.back());
    m_data.back() = 0;
    m_data.pop_back();
}

// Swap two vectors.
template<typename T>
inline void
afl::container::PtrVector<T>::swap(PtrVector<T>& other)
{
    m_data.swap(other.m_data);
}

// Get size of this vector.
template<typename T>
inline typename afl::container::PtrVector<T>::size_type
afl::container::PtrVector<T>::size() const
{
    return m_data.size();
}

// Check emptiness.
template<typename T>
inline bool
afl::container::PtrVector<T>::empty() const
{
    return m_data.empty();
}

// Clear this vector.
template<typename T>
void
afl::container::PtrVector<T>::clear()
{
    deleteAll();
    m_data.clear();
}

// Index access.
template<typename T>
inline T*
afl::container::PtrVector<T>::operator[](size_type index) const
{
    return static_cast<T*>(m_data[index]);
}

// Get first object.
template<typename T>
inline T*
afl::container::PtrVector<T>::front() const
{
    return static_cast<T*>(m_data.front());
}

// Get last object.
template<typename T>
inline T*
afl::container::PtrVector<T>::back() const
{
    return static_cast<T*>(m_data.back());
}

// Get begin iterator.
template<typename T>
inline typename afl::container::PtrVector<T>::const_iterator
afl::container::PtrVector<T>::begin() const
{
    return m_data.begin();
}

// Get end iterator.
template<typename T>
inline typename afl::container::PtrVector<T>::const_iterator
afl::container::PtrVector<T>::end() const
{
    return m_data.end();
}

// Reserve space.
template<typename T>
inline void
afl::container::PtrVector<T>::reserve(size_type size)
{
    m_data.reserve(size);
}

// Erase single element.
template<typename T>
void
afl::container::PtrVector<T>::erase(iterator i)
{
    delete *i;

    // The iterator is a vector<>::const_iterator, convert it to a regular iterator:
    typename std::vector<void*>::iterator ii = i - begin() + m_data.begin();
    m_data.erase(ii);
}

// Erase range.
template<typename T>
void
afl::container::PtrVector<T>::erase(iterator i, iterator j)
{
    for (iterator z = i; z != j; ++z) {
        delete *z;
    }

    // The iterators are vector<>::const_iterator, convert them to regular iterators:
    size_type iindex = i - begin();
    size_type jindex = j - begin();
    m_data.erase(m_data.begin() + iindex, m_data.begin() + jindex);

    // This used to be
    //    typename std::vector<void*>::iterator ii = i - begin() + m_data.begin();
    //    typename std::vector<void*>::iterator jj = j - begin() + m_data.begin();
    //    m_data.erase(ii, jj);
    // but this is miscompiled for PtrVector<GOutbox::Message> by g++-3.3
    // (it apparently turns a '-' into a '+').
}

// Sort.
template<typename T>
void
afl::container::PtrVector<T>::sort()
{
    std::sort(m_data.begin(), m_data.end(), StandardPredicate());
}

// Sort by predicate.
template<typename T>
template<typename Predicate>
void
afl::container::PtrVector<T>::sort(Predicate p)
{
    std::sort(m_data.begin(), m_data.end(), TemplatePredicate<Predicate>(p));
}

// Swap elements.
template<typename T>
inline void
afl::container::PtrVector<T>::swapElements(size_type a, size_type b)
{
    std::swap(m_data[a], m_data[b]);
}

// Swap elements of different PtrVector's.
template<typename T>
inline void
afl::container::PtrVector<T>::swapElements(size_type a, PtrVector& other, size_type b)
{
    std::swap(m_data[a], other.m_data[b]);
}

// Replace an element.
template<typename T>
void
afl::container::PtrVector<T>::replaceElementNew(size_type a, T* value)
{
    typename afl::tmp::CopyCV<T, void>::Type* tv = value;
    delete static_cast<T*>(m_data[a]);
    m_data[a] = const_cast<void*>(tv);
}

// Destructively extract element from this vector.
template<typename T>
inline T*
afl::container::PtrVector<T>::extractElement(size_type a)
{
    T* value = static_cast<T*>(m_data[a]);
    m_data[a] = 0;
    return value;
}

// Destructively extract last element from this vector.
template<typename T>
inline T*
afl::container::PtrVector<T>::extractLast()
{
    T* value = static_cast<T*>(m_data.back());
    m_data.pop_back();
    return value;
}

/** Delete all elements. */
template<typename T>
void
afl::container::PtrVector<T>::deleteAll()
{
    // delete backwards.
    for (size_type i = m_data.size(); i != 0; --i) {
        delete static_cast<T*>(m_data[i-1]);
        m_data[i-1] = 0;
    }
}

#endif
