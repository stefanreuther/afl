/**
  *  \file afl/async/operationlist.hpp
  *  \brief Template class afl::async::OperationList
  */
#ifndef AFL_AFL_ASYNC_OPERATIONLIST_HPP
#define AFL_AFL_ASYNC_OPERATIONLIST_HPP

#include <list>
#include "afl/async/operation.hpp"

namespace afl { namespace async {

    class Controller;

    /** List of Operations.
        This class offers useful operations in lists of Operation objects.

        \todo This is incomplete. It should someday use an intrusive list. */
    template<typename T>
    class OperationList {
     public:
        /** Constructor. Makes an empty list. */
        OperationList();

        /** Destructor. */
        ~OperationList();

        /** Add element at end.
            \param op Element to add */
        void pushBack(T* op);

        /** Remove and return front element.
            \return Front element; null if none */
        T* extractFront();

        /** Get front element.
            \return Front element; null if none */
        T* front() const;

        /** Check emptiness.
            \return true iff list is empty. */
        bool empty() const;

        /** Remove and return element for a given controller.
            \param ctl Controller
            \return An element whose getController() returns ctl; null if no such element */
        T* extractByController(Controller* ctl);

        /** Remove an element by pointer.
            \param op Element to remove
            \retval true element was removed
            \retval false element was not contained in this list */
        bool remove(Operation* op);

     private:
        std::list<Operation*> m_list;
    };
    
} }

/***************************** Implementation ****************************/

template<typename T>
afl::async::OperationList<T>::OperationList()
{ }

template<typename T>
afl::async::OperationList<T>::~OperationList()
{ }

template<typename T>
inline void
afl::async::OperationList<T>::pushBack(T* op)
{
    m_list.push_back(op);
}

template<typename T>
T*
afl::async::OperationList<T>::extractFront()
{
    T* result = 0;
    if (!m_list.empty()) {
        result = static_cast<T*>(m_list.front());
        m_list.pop_front();
    }
    return result;
}

template<typename T>
T*
afl::async::OperationList<T>::front() const
{
    T* result = 0;
    if (!m_list.empty()) {
        result = static_cast<T*>(m_list.front());
    }
    return result;
}

template<typename T>
inline bool
afl::async::OperationList<T>::empty() const
{
    return m_list.empty();
}

template<typename T>
T*
afl::async::OperationList<T>::extractByController(Controller* ctl)
{
    T* result = 0;
    std::list<Operation*>::iterator p = m_list.begin();
    while (p != m_list.end() && (*p)->getController() != ctl) {
        ++p;
    }
    if (p != m_list.end()) {
        result = static_cast<T*>(*p);
        m_list.erase(p);
    }
    return result;
}

template<typename T>
bool
afl::async::OperationList<T>::remove(Operation* op)
{
    std::list<Operation*>::iterator p = m_list.begin();
    while (p != m_list.end() && *p != op) {
        ++p;
    }
    if (p != m_list.end()) {
        m_list.erase(p);
        return true;
    } else {
        return false;
    }
}

#endif
