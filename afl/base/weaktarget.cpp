/**
  *  \file afl/base/weaktarget.cpp
  *  \brief Base class afl::base::WeakTarget
  */

#include "afl/base/weaktarget.hpp"
#include "afl/base/baseweaklink.hpp"

// Add a WeakLink instance that points here.
void
afl::base::WeakTarget::add(BaseWeakLink& link)
{
    m_links.insert(&link);
}

// Remove a WeakLink instance that pointed here.
void
afl::base::WeakTarget::remove(BaseWeakLink& link)
{
    m_links.erase(&link);
}

// Constructor.
afl::base::WeakTarget::WeakTarget()
    : m_links()
{ }

// Destructor.
afl::base::WeakTarget::~WeakTarget()
{
    for (std::set<BaseWeakLink*>::iterator it = m_links.begin(), end = m_links.end(); it != end; ++it) {
        (*it)->disconnect();
    }
}
