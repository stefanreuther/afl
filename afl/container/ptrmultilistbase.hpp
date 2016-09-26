/**
  *  \file afl/container/ptrmultilistbase.hpp
  *  \brief Class afl::container::PtrMultiListBase
  */
#ifndef AFL_AFL_CONTAINER_PTRMULTILISTBASE_HPP
#define AFL_AFL_CONTAINER_PTRMULTILISTBASE_HPP

#include <algorithm>
#include "afl/base/types.hpp"
#include "afl/base/uncopyable.hpp"

namespace afl { namespace container {

    /** List of pointers with multiple iterators, basic version.
        Implements ownership semantics for a list of pointers.
        In addition, allows multiple iterators into it,
        where deletion and/or insertion invalidates neither.

        This is the underlying implementation of PtrMultiList<T>.
        It takes its parameterisation through a function pointer.
        It can be used standalone, but normally shouldn't.
        Its interface isn't as full/conventional as the typed
        PtrMultiList<T>'s.

        Multiple iterators can point at PtrMultiListBase's elements.
        If one iterator is used to delete an element, other iterators
        pointing at that element will return null. */
    class PtrMultiListBase : public afl::base::Uncopyable {
        struct Node;

     public:
        /** Iterator. A forward iterator. */
        class iterator {
         public:
            friend class PtrMultiListBase;

            iterator();
            iterator(Node* item, iterator** pThis);
            ~iterator();
            iterator(const iterator& other);
            iterator& operator=(const iterator& other);
            bool valid() const;
            void next();
            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const;
            void* get() const;
            void invalidate();
            
         private:
            void unlink();
            void link(const iterator& other);

            Node*      m_item;
            Node*      m_nextItem;
            mutable iterator*  m_nextIterator;
            mutable iterator** m_pThisIterator;
        };

        explicit PtrMultiListBase(void m_deleter(void*));
        ~PtrMultiListBase();
        void clear();
        bool empty() const;
        void pushBackNew(void* data);
        void pushFrontNew(void* data);
        void erase(const iterator& iter);
        iterator insert(const iterator& iter, void* data);
        void swapElements(const iterator& a, const iterator& b);
        void* extractElement(const iterator& iter);
        iterator begin() const;
        iterator end() const;
        void* front() const;
        void* extractFront();
        size_t size() const;
        void invalidateAllIterators();

     private:
        Node* insertAtEnd(void* data);
        Node* insertBefore(Node* item, void* data);

        struct Node {
            Node*  m_nextNode;
            Node** m_pThisNode;
            void*  m_data;
        };
        Node*     m_firstNode;
        Node**    m_pLastNode;
        mutable iterator* m_firstIterator;
        void    (*m_deleter)(void*);
    };

} } // afl::container

/** Constructor. */
inline
afl::container::PtrMultiListBase::iterator::iterator()
    : m_item(0),
      m_nextItem(0),
      m_nextIterator(0),
      m_pThisIterator(0)
{ }

/** Constructor. Builds the iterator and registers it in a list.
    \param item  Item to point at
    \param pThis Place pointer to this iterator at this place */
inline
afl::container::PtrMultiListBase::iterator::iterator(Node* item, iterator** pThis)
    : m_item(item),
      m_nextItem(item ? item->m_nextNode : 0),
      m_nextIterator(*pThis),
      m_pThisIterator(pThis)
{
    *m_pThisIterator = this;
    if (m_nextIterator) {
        m_nextIterator->m_pThisIterator = &m_nextIterator;
    }
}

/** Destructor. */
inline
afl::container::PtrMultiListBase::iterator::~iterator()
{
    unlink();
}

/** Copy constructor. */
inline
afl::container::PtrMultiListBase::iterator::iterator(const iterator& other)
    : m_item(other.m_item),
      m_nextItem(other.m_nextItem)
{
    link(other);
}

/** Check validity. The iterator is valid if it points at an undeleted item.
    Corrolary: it is invalid if it points at a deleted item, or after the
    end of the sequence. */
inline bool
afl::container::PtrMultiListBase::iterator::valid() const
{
    return m_item != 0;
}

/** Advance to next item. */
inline void
afl::container::PtrMultiListBase::iterator::next()
{
    m_item = m_nextItem;
    if (m_nextItem != 0) {
        m_nextItem = m_item->m_nextNode;
    }
}

/** Compare equality. */
inline bool
afl::container::PtrMultiListBase::iterator::operator==(const iterator& other) const
{
    return other.m_item == m_item && other.m_nextItem == m_nextItem;
}

/** Compare inequality. */
inline bool
afl::container::PtrMultiListBase::iterator::operator!=(const iterator& other) const
{
    return !operator==(other);
}

/** Get referenced item.
    The result will be null if this iterator points at a deleted item. */
inline void*
afl::container::PtrMultiListBase::iterator::get() const
{
    return m_item ? m_item->m_data : 0;
}

/** Invalidate this iterator. Turns it into an unassociated end() iterator. */
inline void
afl::container::PtrMultiListBase::iterator::invalidate()
{
    this->unlink();
    m_item = 0;
    m_nextItem = 0;
}

/** Constructor.
    \param deleter Function to delete elements. */
inline
afl::container::PtrMultiListBase::PtrMultiListBase(void deleter(void*))
    : m_firstNode(0),
      m_pLastNode(&m_firstNode),
      m_firstIterator(0),
      m_deleter(deleter)
{ }

/** Destructor. Clears the list. */
inline
afl::container::PtrMultiListBase::~PtrMultiListBase()
{
    clear();
}

/** Check emptiness.
    \return true iff this list is empty */
inline bool
afl::container::PtrMultiListBase::empty() const
{
    return m_firstNode == 0;
}

/** Append an element.
    If appending fails, deletes the element before rethrowing the exception. */
inline void
afl::container::PtrMultiListBase::pushBackNew(void* data)
{
    insertAtEnd(data);
}

/** Swap elements by iterator.
    Both iterators must point at undeleted elements. */
inline void
afl::container::PtrMultiListBase::swapElements(const iterator& a, const iterator& b)
{
    std::swap(a.m_item->m_data, b.m_item->m_data);
}

/** Get iterator to first element. */
inline afl::container::PtrMultiListBase::iterator
afl::container::PtrMultiListBase::begin() const
{
    return iterator(m_firstNode, &m_firstIterator);
}

/** Get iterator to one-past-last element. */
inline afl::container::PtrMultiListBase::iterator
afl::container::PtrMultiListBase::end() const
{
    return iterator();
}

/** Get first element. */
inline void*
afl::container::PtrMultiListBase::front() const
{
    return m_firstNode->m_data;
}

/** Extract first element. Removes the first element and returns it. */
inline void*
afl::container::PtrMultiListBase::extractFront()
{
    return extractElement(begin());
}

#endif
