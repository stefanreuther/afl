/**
  *  \file afl/container/ptrmultilistbase.cpp
  *  \brief Class afl::container::PtrMultiListBase
  *
  *  This class is implemented using almost doubly-linked lists:
  *  - a list of elements
  *  - a list of live iterators
  *
  *  The almost doubly-linked lists stores a pointer to the next element (next_iterator),
  *  as well as a pointer to the pointer to this element (pthis_iterator). This allows
  *  easy prepend, append, and removal of elements, but no backward traversal.
  *
  *  Each iterator contains a pointer to the current element as well as one to the next one.
  *  When an element is deleted, we can thus simply keep a reference into the list.
  */

#include <cassert>
#include "afl/container/ptrmultilistbase.hpp"

/** Assign iterator. */
afl::container::PtrMultiListBase::iterator&
afl::container::PtrMultiListBase::iterator::operator=(const afl::container::PtrMultiListBase::iterator& other)
{
    if (this != &other) {
        unlink();
        m_item = other.m_item;
        m_nextItem = other.m_nextItem;
        link(other);
    }
    return *this;
}

/** Internal: unlink this iterator. It will no longer be associated with a list. */
void
afl::container::PtrMultiListBase::iterator::unlink()
{
    if (m_pThisIterator != 0) {
        // Unlink
        assert(*m_pThisIterator == this);
        *m_pThisIterator = m_nextIterator;
        if (m_nextIterator != 0) {
            m_nextIterator->m_pThisIterator = m_pThisIterator;
        }

        // Make operation idempotent
        m_pThisIterator = 0;
        m_nextIterator = 0;
    }
}

/** Internal: link this iterator. Associates it with the same list as other. */
void
afl::container::PtrMultiListBase::iterator::link(const iterator& other)
{
    if (other.m_pThisIterator != 0) {
        // The other one is a live iterator
        m_nextIterator = const_cast<iterator*>(&other);
        m_pThisIterator = other.m_pThisIterator;
        *m_pThisIterator = this;
        other.m_pThisIterator = &m_nextIterator;
    } else {
        // The other one is not a live iterator
        m_nextIterator = 0;
        m_pThisIterator = 0;
    }
}

/** Clear list. Deletes all elements. */
void
afl::container::PtrMultiListBase::clear()
{
    // First invalidate iterators, to save work later on erase()
    invalidateAllIterators();

    // Clear the container. This could actually be implemented even simpler.
    while (!empty()) {
        m_deleter(extractFront());
    }
}

/** Prepend an element.
    If appending fails, deletes the element before rethrowing the exception. */
void
afl::container::PtrMultiListBase::pushFrontNew(void* data)
{
    try {
        Node* n = new Node;
        n->m_data = data;
        n->m_nextNode = m_firstNode;
        n->m_pThisNode = &m_firstNode;
        if (m_firstNode == 0) {
            m_pLastNode = &n->m_nextNode;
        }
        m_firstNode = n;
    }
    catch (...) {
        m_deleter(data);
        throw;
    }
}

/** Erase element. Deletes it in the process. */
void
afl::container::PtrMultiListBase::erase(const iterator& iter)
{
    m_deleter(extractElement(iter));
}

/** Insert element before. */
afl::container::PtrMultiListBase::iterator
afl::container::PtrMultiListBase::insert(const iterator& iter, void* data)
{
    Node* result;
    if (iter.m_item == 0) {
        if (iter.m_nextItem == 0) {
            // Case 1: iterator points at end, so this is pushBack.
            result = insertAtEnd(data);
        } else {
            // Case 2: iterator points at a deleted element.
            // Insert before nextItem.
            result = insertBefore(iter.m_nextItem, data);
        }
    } else {
        // Case 3: iterator is valid. Insert before item.
        result = insertBefore(iter.m_item, data);
    }
    return iterator(result, &m_firstIterator);
}

/** Extract element. Removes an element from the sequence and returns it. */
void*
afl::container::PtrMultiListBase::extractElement(const iterator& iter)
{
    void* result;
    Node*const item = iter.m_item;
    if (item != 0) {
        // Erase iterators
        for (iterator* i = m_firstIterator; i != 0; i = i->m_nextIterator) {
            if (item == i->m_item) {
                i->m_item = 0;
            } else if (item == i->m_nextItem) {
                i->m_nextItem = i->m_nextItem->m_nextNode;
            }
        }

        // Delete item
        *item->m_pThisNode = item->m_nextNode;
        if (item->m_nextNode) {
            item->m_nextNode->m_pThisNode = item->m_pThisNode;
        } else {
            m_pLastNode = item->m_pThisNode;
        }
        result = item->m_data;
        delete item;
    } else {
        result = 0;
    }
    return result;
}

/** Invalidate all iterators. Turns them all into end() iterators. */
void
afl::container::PtrMultiListBase::invalidateAllIterators()
{
    while (m_firstIterator != 0) {
        m_firstIterator->invalidate();
    }
}

/** Insert a new element at the end. */
afl::container::PtrMultiListBase::Node*
afl::container::PtrMultiListBase::insertAtEnd(void* data)
{
    try {
        Node* n = new Node;
        n->m_data = data;
        n->m_nextNode = 0;
        n->m_pThisNode = m_pLastNode;
        *m_pLastNode = n;
        m_pLastNode = &n->m_nextNode;
        return n;
    }
    catch (...) {
        m_deleter(data);
        throw;
    }
}

/** Insert a new element before an item. */
afl::container::PtrMultiListBase::Node*
afl::container::PtrMultiListBase::insertBefore(Node* item, void* data)
{
    assert(item != 0);
    try {
        // Insert new node
        Node* n = new Node;
        n->m_data = data;
        n->m_nextNode = item;
        n->m_pThisNode = item->m_pThisNode;
        *n->m_pThisNode = n;
        item->m_pThisNode = &n->m_nextNode;

        // Rewrite iterators
        // Do not rewrite iterators pointing at a deleted item.
        // In particular, this could be the iterator used to insert the element.
        for (iterator* i = m_firstIterator; i != 0; i = i->m_nextIterator) {
            if (i->m_item != 0 && i->m_nextItem == item) {
                i->m_nextItem = n;
            }
        }
        return n;
    }
    catch (...) {
        m_deleter(data);
        throw;
    }
}

/** Get size of list. */
size_t
afl::container::PtrMultiListBase::size() const
{
    size_t result = 0;
    for (Node* n = m_firstNode; n != 0; n = n->m_nextNode) {
        ++result;
    }
    return result;
}
