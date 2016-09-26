/**
  *  \file afl/base/deleter.cpp
  *  \brief Class afl::base::Deleter
  */

#include <cassert>
#include "afl/base/deleter.hpp"

// Constructor. Makes empty deleter.
afl::base::Deleter::Deleter()
    : Deletable(),
      Uncopyable(),
      m_list()
{ }

// Destructor.
afl::base::Deleter::~Deleter()
{
    clear();
}

// Add a new object without error handling.
void
afl::base::Deleter::unsafeAddNew(Deletable* p)
{
    add(p);
}

// Transfer ownership
void
afl::base::Deleter::moveTo(Deleter& other)
{
    if (&other != this) {
        try {
            for (List_t::iterator i = m_list.begin(), e = m_list.end(); i != e; ++i) {
                other.unsafeAddNew(*i);
                *i = 0;
            }
            m_list.clear();
        }
        catch (...) {
            clear();
            throw;
        }
    }
}

// Clear, deleting content.
void
afl::base::Deleter::clear()
{
    while (!m_list.empty()) {
        delete m_list.back();
        m_list.back() = 0;
        m_list.pop_back();
    }
    m_list.clear();
}

// Add, back-end.
void
afl::base::Deleter::add(Deletable* p)
{
    try {
        assert(p != 0);
        m_list.push_back(p);
    }
    catch (...) {
        delete p;
        throw;
    }
}
