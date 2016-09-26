/**
  *  \file afl/container/ptrmap.hpp
  *  \brief Template Class afl::container::PtrMap
  *
  *  Ported from cpluslib/ptrmap.h 1.3.
  */
#ifndef AFL_AFL_CONTAINER_PTRMAP_HPP
#define AFL_AFL_CONTAINER_PTRMAP_HPP

#include <map>
#include <iterator>
#include "afl/base/uncopyable.hpp"
#include "afl/tmp/copycv.hpp"

namespace afl { namespace container {

    /** Map of Pointers. Implements ownership semantics for a map of pointer
        values, and attempts to manage it in a type- and exception-safe way.

        The basic assumption is that PtrMap<> owns the values passed to it
        (keys must still manage themselves). It will delete them when the slot
        holding the pointer goes away. You must not delete a pointer obtained
        from PtrMap<>, or place it in another PtrMap<>.

        Otherwise, the interface attempts to be somewhat close to
        std::map<>, restricted to operations that are safe enough to
        maintain ownership semantics. */
    template<typename Key, typename T>
    class PtrMap : public afl::base::Uncopyable {
     public:
        class iterator;
        typedef iterator const_iterator;
        typedef typename std::map<Key,void*>::size_type size_type;
        typedef typename std::map<Key,void*>::difference_type difference_type;
        typedef std::pair<Key, T*> value_type;

        PtrMap();
        ~PtrMap();

        iterator begin() const;
        iterator end() const;

        T* insertNew(const Key& k, T* p);
        void swap(PtrMap& other);
        void swapElements(iterator a, iterator b);
        T* extractElement(iterator i);
        size_type size() const;
        bool empty() const;
        void clear();
        T* operator[](const Key& k) const;

        iterator find(const Key& k) const;
        iterator lower_bound(const Key& k) const;
        iterator upper_bound(const Key& k) const;
        size_type count(const Key& k) const;
        void erase(iterator a, iterator e);
        void erase(iterator i);
        size_type erase(const Key& k);

     private:
        /** Content. Implemented by mapping to void*, to share implementation. */
        std::map<Key,void*> m_data;
        
        void deleteAll();
    };

} }

/** Bidirectional iterator. Supports
    - post/pre increment/decrement
    - addition and subtraction of difference_type
    - subtraction of another iterator
    - indexing
    - dereferencing
    - comparison */
template<typename Key, typename T>
class afl::container::PtrMap<Key,T>::iterator
    : public std::iterator<std::bidirectional_iterator_tag,
                           const typename PtrMap<Key,T>::value_type,
                           typename PtrMap<Key,T>::difference_type,
                           const typename PtrMap<Key,T>::value_type*,
                           const typename PtrMap<Key,T>::value_type&>
{
    friend class PtrMap<Key,T>;
    typename std::map<Key,void*>::iterator m_it;
    struct Proxy {
        const Key& first;
        T* const second;
        Proxy(const std::pair<const Key, void*>& orig)
            : first(orig.first), second(static_cast<T*>(orig.second))
            { }
        Proxy* operator->()
            { return this; }
        const Proxy* operator->() const
            { return this; }
    };
 public:
    typedef typename std::map<Key,void*>::difference_type difference_type;

    // Constructor
    iterator()
        : m_it() { }
    iterator(typename std::map<Key,void*>::iterator it)
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

    // Comparison
    bool operator==(iterator rhs) const
        { return m_it == rhs.m_it; }
    bool operator!=(iterator rhs) const
        { return m_it != rhs.m_it; }

    // Dereferencing
    Proxy operator*() const
        { return Proxy(*m_it); }
    Proxy operator->() const
        { return Proxy(*m_it); }
};


/** Constructor. Makes a blank map. */
template<typename Key, typename T>
afl::container::PtrMap<Key,T>::PtrMap()
    : afl::base::Uncopyable(),
      m_data()
{ }

/** Destructor. Deletes all contained elements. */
template<typename Key, typename T>
afl::container::PtrMap<Key,T>::~PtrMap()
{
    deleteAll();
}

/** Delete all contained elements. */
template<typename Key, typename T>
void
afl::container::PtrMap<Key,T>::deleteAll()
{
    for (typename std::map<Key,void*>::iterator i = m_data.begin(); i != m_data.end(); ++i)
        delete static_cast<T*>(i->second);
}

/** Iterator at beginning of map. */
template<typename Key, typename T>
inline typename afl::container::PtrMap<Key,T>::iterator
afl::container::PtrMap<Key,T>::begin() const
{
    return iterator(const_cast<std::map<Key,void*>&>(m_data).begin());
}

/** Iterator at end of map. */
template<typename Key, typename T>
inline typename afl::container::PtrMap<Key,T>::iterator
afl::container::PtrMap<Key,T>::end() const
{
    return iterator(const_cast<std::map<Key,void*>&>(m_data).end());
}

/** Insert new element. Takes ownership of p. If anything throws, p is deleted.
    \todo Note that this is currently not the standard std::map insert() signature.
    \param k Key to create/replace
    \param p Value to store */
template<typename Key, typename T>
T*
afl::container::PtrMap<Key,T>::insertNew(const Key& k, T* p)
{
    // Not too efficient, but works for the time being
    typename afl::tmp::CopyCV<T, void>::Type* pv = p;
    try {
        delete operator[](k);
        m_data[k] = const_cast<void*>(pv);
        return p;
    }
    catch (...) {
        delete p;
        throw;
    }
}

/** Swap with another map. */
template<typename Key, typename T>
inline void
afl::container::PtrMap<Key,T>::swap(PtrMap<Key,T>& other)
{
    m_data.swap(other.m_data);
}

/** Swap some elements. The iterators must not be end(). */
template<typename Key, typename T>
inline void
afl::container::PtrMap<Key,T>::swapElements(iterator a, iterator b)
{
    void* p = a.m_it->second;
    a.m_it->second = b.m_it->second;
    b.m_it->second = p;
}

/** Extract an element. Returns the element. The stored element will be erased. */
template<typename Key, typename T>
inline T*
afl::container::PtrMap<Key,T>::extractElement(iterator i)
{
    T* p = static_cast<T*>(i.m_it->second);
    m_data.erase(i.m_it);
    return p;
}

/** Get number of elements in this map. */
template<typename Key, typename T>
inline typename afl::container::PtrMap<Key,T>::size_type
afl::container::PtrMap<Key,T>::size() const
{
    return m_data.size();
}

/** Check for emptiness.
    \return true iff map is empty. */
template<typename Key, typename T>
inline bool
afl::container::PtrMap<Key,T>::empty() const
{
    return m_data.empty();
}

/** Clear this map. Deletes all contained value pointers. */
template<typename Key, typename T>
inline void
afl::container::PtrMap<Key,T>::clear()
{
    deleteAll();
    m_data.clear();
}

/** Element access. Note that unlike a std::map, a PtrMap can only be read this way
    (on the plus side, using operator[] to check for an element will not create it).
    \param k Key to access */
template<typename Key, typename T>
T*
afl::container::PtrMap<Key,T>::operator[](const Key& k) const
{
    typename std::map<Key,void*>::const_iterator it = m_data.find(k);
    if (it != m_data.end())
        return static_cast<T*>(it->second);
    else
        return 0;
}

/** Find element by key. Returns an iterator pointing to an element whose
    key is k. Returns end() if there is no such element. */
template<typename Key, typename T>
inline typename afl::container::PtrMap<Key,T>::iterator
afl::container::PtrMap<Key,T>::find(const Key& k) const
{
    return iterator(const_cast<std::map<Key,void*>&>(m_data).find(k));
}

/** Find element at or after key.
    Returns an iterator pointing to the first element whose key is >= \c k.
    Returns end() if there is no such element (all elements are less). */
template<typename Key, typename T>
inline typename afl::container::PtrMap<Key,T>::iterator
afl::container::PtrMap<Key,T>::lower_bound(const Key& k) const
{
    return iterator(const_cast<std::map<Key,void*>&>(m_data).lower_bound(k));
}

/** Find element before key.
    Returns an iterator pointing to an element whose key is < \c k.
    Returns end() if there is no such element (all elements are greater or equal \c k). */
template<typename Key, typename T>
inline typename afl::container::PtrMap<Key,T>::iterator
afl::container::PtrMap<Key,T>::upper_bound(const Key& k) const
{
    return iterator(const_cast<std::map<Key,void*>&>(m_data).upper_bound(k));
}

/** Count elements by key. Returns the number of elements whose key is k.
    For a map, this is either 0 or 1. */
template<typename Key, typename T>
inline typename afl::container::PtrMap<Key,T>::size_type
afl::container::PtrMap<Key,T>::count(const Key& k) const
{
    return m_data.count(k);
}

/** Erase elements given a range. Erases all elements from a
    (includive) up to e (not inclusive). */
template<typename Key, typename T>
inline void
afl::container::PtrMap<Key,T>::erase(iterator a, iterator e)
{
    while (a != e) {
        iterator m = a;
        ++a;
        this->erase(m);
    }
}

/** Erase element given an iterator. Erases the element pointed-to by
    the iterator. */
template<typename Key, typename T>
void
afl::container::PtrMap<Key,T>::erase(iterator i)
{
    delete i->second;
    m_data.erase(i.m_it);
}

/** Erase element given a key. Erases all elements whose key equals k.
    Returns the number of elements erased. */
template<typename Key, typename T>
typename afl::container::PtrMap<Key,T>::size_type
afl::container::PtrMap<Key,T>::erase(const Key& k)
{
    iterator i = this->find(k);
    if (i != this->end()) {
        this->erase(i);
        return 1;
    } else {
        return 0;
    }
}

#endif
