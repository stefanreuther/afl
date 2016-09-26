/**
  *  \file afl/container/ptrqueue.hpp
  *  \brief Template class afl::container::PtrQueue
  */
#ifndef AFL_AFL_CONTAINER_PTRQUEUE_HPP
#define AFL_AFL_CONTAINER_PTRQUEUE_HPP

#include <deque>
#include "afl/base/uncopyable.hpp"
#include "afl/tmp/copycv.hpp"

namespace afl { namespace container {

    /** Queue of pointers.
        Implements ownership semantics for a queue of pointers, and manages them in an exception-safe and type-safe way.

        PtrQueue<> will own objects passed to it.
        When removing an element, you can decide whether to delete it (popFront()) or put it into your responsibility (extractFront());
        clearing the queue will delete all elements.

        It is well-defined to put null pointers into a queue.
        However, restricting usage to non-null pointers allows for some interesting coding patterns
        (such as "while (T* t = q.extractFront())").

        The interface attempts to be somewhat close to std::queue<>. */
    template<typename T>
    class PtrQueue : private afl::base::Uncopyable {
     public:
        /** Type for number-of-elements. */
        typedef std::deque<void*>::size_type size_type;

        /** Type for contained element. */
        typedef T* value_type;

        /** Constructor. Makes an empty queue. */
        PtrQueue();

        /** Destructor. Destroy all contained objects. */
        ~PtrQueue();

        /** Add object at end.
            The pointed object becomes owned by this queue.
            If this function throws (due to lack of memory to expand the queue), it will delete \c t.
            \param t object to add (can be null)
            \return t */
        void pushBackNew(T* t);

        /** Remove oldest object.
            Deletes the pointed-to object, if any.
            If the queue is empty, this function does nothing. */
        void popFront();

        /** Remove oldest object, returning it.
            Returns the pointer. You receive ownership of the object.
            \return pointer to oldest object, null if the queue is empty */
        T* extractFront();

        /** Get newest object.
            \return last pointer added with pushBackNew(), null if none */
        T* back();

        /** Get oldest object.
            \return oldest pointer added with pushBackNew(), null if none */
        T* front();

        /** Clear the queue.
            Deletes all objects. */
        void clear();

        /** Check emptiness.
            \return true iff queue is empty */
        bool empty() const;

        /** Get queue size.
            \return number of objects in queue */
        size_type size() const;

        /** Swap queues.
            \param other Other queue */
        void swap(PtrQueue& other);

     private:
        // We base this on deque, not queue.
        // queue is in turn based on deque anyway, but has a more restricted interface.
        std::deque<void*> m_data;
    };

} }


// Constructor.
template<typename T>
afl::container::PtrQueue<T>::PtrQueue()
    : m_data()
{ }

// Destructor.
template<typename T>
afl::container::PtrQueue<T>::~PtrQueue()
{
    this->clear();
}

// Add object at end.
template<typename T>
void
afl::container::PtrQueue<T>::pushBackNew(T* t)
{
    typename afl::tmp::CopyCV<T, void>::Type* tv = t;
    try {
        m_data.push_back(const_cast<void*>(tv));
    }
    catch (...) {
        delete t;
        throw;
    }
}

// Remove oldest object.
template<typename T>
void
afl::container::PtrQueue<T>::popFront()
{
    delete extractFront();
}

// Remove oldest object, returning it.
template<typename T>
T*
afl::container::PtrQueue<T>::extractFront()
{
    T* result;
    if (m_data.empty()) {
        result = 0;
    } else {
        result = static_cast<T*>(m_data.front());
        m_data.pop_front();
    }
    return result;
}

// Get newest object.
template<typename T>
T*
afl::container::PtrQueue<T>::back()
{
    return m_data.empty() ? 0 : static_cast<T*>(m_data.back());
}

// Get oldest object.
template<typename T>
T*
afl::container::PtrQueue<T>::front()
{
    return m_data.empty() ? 0 : static_cast<T*>(m_data.front());
}

// Clear the queue.
template<typename T>
void
afl::container::PtrQueue<T>::clear()
{
    while (!m_data.empty()) {
        delete static_cast<T*>(m_data.front());
        m_data.pop_front();
    }
}

// Check emptiness.
template<typename T>
inline bool
afl::container::PtrQueue<T>::empty() const
{
    return m_data.empty();
}

// Get queue size.
template<typename T>
inline typename afl::container::PtrQueue<T>::size_type
afl::container::PtrQueue<T>::size() const
{
    return m_data.size();
}

// Swap queues.
template<typename T>
inline void
afl::container::PtrQueue<T>::swap(PtrQueue& other)
{
    m_data.swap(other.m_data);
}

#endif
