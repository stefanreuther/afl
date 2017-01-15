/**
  *  \file afl/base/baseweaklink.cpp
  *  \brief Internal class afl::base::BaseWeakLink
  */

#include "afl/base/baseweaklink.hpp"

// Initialize from pointer.
afl::base::BaseWeakLink::BaseWeakLink(WeakTarget* target)
    : m_target(0)
{
    init(target);
}

// Copy constructor.
afl::base::BaseWeakLink::BaseWeakLink(const BaseWeakLink& other)
    : m_target(0)
{
    init(other.get());
}

// Destructor.
afl::base::BaseWeakLink::~BaseWeakLink()
{
    reset();
}

// Set to different pointer.
void
afl::base::BaseWeakLink::reset(WeakTarget* target)
{
    reset();
    init(target);
}

// Set to null.
void
afl::base::BaseWeakLink::reset()
{
    if (m_target != 0) {
        m_target->remove(*this);
        m_target = 0;
    }
}

// Get contained pointer.
afl::base::WeakTarget*
afl::base::BaseWeakLink::get() const
{
    return m_target;
}


void
afl::base::BaseWeakLink::disconnect()
{
    m_target = 0;
}

void
afl::base::BaseWeakLink::init(WeakTarget* target)
{
    if (target != 0) {
        m_target = target;
        m_target->add(*this);
    }
}
