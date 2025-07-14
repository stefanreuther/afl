/**
  *  \file afl/io/multiplexablestream.cpp
  *  \brief Class afl::io::MultiplexableStream
  */

#include <cassert>
#include "afl/io/multiplexablestream.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/mutexguard.hpp"

/** Control Node.
    Manages the parent and children.
    All methods must be called with the mutex held. */
class afl::io::MultiplexableStream::ControlNode : public afl::base::RefCounted {
 public:
    /** Constructor. */
    ControlNode(MultiplexableStream* parent);

    /** Destructor. */
    ~ControlNode();

    /** Mutex. */
    afl::sys::Mutex m_mutex;

    /** Add a child.
        Must be called with m_mutex held.
        \param ch The child */
    void addChild(Child* ch);

    /** Remove a child.
        Must be called with m_mutex held.
        If the child is active, it will be deactivated.
        \param ch The child */
    void removeChild(Child* ch);

    /** Remove the parent.
        Must be called with m_mutex held. */
    void removeParent();

    /** Activate a child.
        Must be called with m_mutex held.
        Performs all necessary checks and decides whether a stateful operation can be performed on the master stream.
        \param ch The child
        \param wantSetPos Set to true if the next operation will be a setPos()
        \return Stream to perform the requested operation on. Null if operation cannot be performed. */
    Stream* activateChild(Child* ch, bool wantSetPos);

    /** Get parent.
        Must be called with m_mutex held.
        Performs all necessary checks and decides whether a stateless operation can be performed on the master stream.
        \return Stream to perform the requested operation on. Null if operation cannot be performed. */
    Stream* getParent();

 private:
    MultiplexableStream* m_parent;
    Child* m_firstChild;
    Child* m_activeChild;
};

/** Child stream.
    Implements a child of a master stream, linked to that using a ControlNode. */
class afl::io::MultiplexableStream::Child : public afl::io::Stream {
    friend class ControlNode;

 public:
    Child(afl::base::Ref<ControlNode> controlNode, uint32_t flags);
    virtual ~Child();

    virtual size_t read(Bytes_t m);
    virtual size_t write(ConstBytes_t m);
    virtual void flush();
    virtual void setPos(FileSize_t pos);
    virtual FileSize_t getPos();
    virtual FileSize_t getSize();
    virtual uint32_t getCapabilities();
    virtual String_t getName();
    virtual afl::base::Ref<Stream> createChild(uint32_t flags);
    virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit);

 private:
    afl::base::Ref<ControlNode> m_controlNode;
    FileSize_t m_posIfInactive;
    uint32_t m_flags;

    // Managed by ControlNode:
    Child* m_nextChild;
    Child** m_pPrevChild;
};


/******************** MultiplexableStream::ControlNode *******************/

inline
afl::io::MultiplexableStream::ControlNode::ControlNode(MultiplexableStream* parent)
    : m_parent(parent),
      m_firstChild(0),
      m_activeChild(0)
{ }

afl::io::MultiplexableStream::ControlNode::~ControlNode()
{
    assert(m_parent == 0);
    assert(m_firstChild == 0);
    assert(m_activeChild == 0);
}

inline void
afl::io::MultiplexableStream::ControlNode::addChild(Child* ch)
{
    ch->m_nextChild = m_firstChild;
    if (m_firstChild != 0) {
        m_firstChild->m_pPrevChild = &ch->m_nextChild;
    }
    m_firstChild = ch;
    ch->m_pPrevChild = &m_firstChild;
}

inline void
afl::io::MultiplexableStream::ControlNode::removeChild(Child* ch)
{
    *ch->m_pPrevChild = ch->m_nextChild;
    if (ch->m_nextChild) {
        ch->m_nextChild->m_pPrevChild = ch->m_pPrevChild;
    }
    ch->m_pPrevChild = 0;
    ch->m_nextChild = 0;
    if (ch == m_activeChild) {
        m_activeChild = 0;
    }
}

inline void
afl::io::MultiplexableStream::ControlNode::removeParent()
{
    m_parent = 0;
    m_activeChild = 0;
}

afl::io::Stream*
afl::io::MultiplexableStream::ControlNode::activateChild(Child* ch, bool wantSetPos)
{
    if (m_parent != 0) {
        if (ch != m_activeChild) {
            // It's not the active child, but we can activate it.
            if (m_activeChild != 0) {
                m_activeChild->m_posIfInactive = m_parent->getPos();
                m_activeChild = 0;
            }
            if (ch != 0 && !wantSetPos) {
                m_parent->setPos(ch->m_posIfInactive);
            }

            // Do this last, in case setPos above fails.
            m_activeChild = ch;
        }
        return m_parent;
    } else {
        // The parent is gone. We cannot perform the operation.
        return 0;
    }
}

inline afl::io::Stream*
afl::io::MultiplexableStream::ControlNode::getParent()
{
    return m_parent;
}

/*********************** MultiplexableStream::Child **********************/

afl::io::MultiplexableStream::Child::Child(afl::base::Ref<ControlNode> controlNode, uint32_t flags)
    : Stream(),
      m_controlNode(controlNode),
      m_posIfInactive(0),
      m_flags(flags),
      m_nextChild(0),
      m_pPrevChild(0)
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    m_controlNode->addChild(this);
}

afl::io::MultiplexableStream::Child::~Child()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    m_controlNode->removeChild(this);
}

size_t
afl::io::MultiplexableStream::Child::read(Bytes_t m)
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->activateChild(this, false)) {
        return w->read(m);
    } else {
        return 0;
    }
}

size_t
afl::io::MultiplexableStream::Child::write(ConstBytes_t m)
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if ((m_flags & DisableWrite) != 0) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::cannotWrite());
    } else if (Stream* w = m_controlNode->activateChild(this, false)) {
        return w->write(m);
    } else {
        return 0;
    }
}

void
afl::io::MultiplexableStream::Child::flush()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->getParent()) {
        w->flush();
    }
}

void
afl::io::MultiplexableStream::Child::setPos(FileSize_t pos)
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->activateChild(this, true)) {
        w->setPos(pos);
    }
}

afl::io::MultiplexableStream::Child::FileSize_t
afl::io::MultiplexableStream::Child::getPos()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->activateChild(this, false)) {
        return w->getPos();
    } else {
        return 0;
    }
}

afl::io::MultiplexableStream::Child::FileSize_t
afl::io::MultiplexableStream::Child::getSize()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->getParent()) {
        return w->getSize();
    } else {
        return 0;
    }
}

uint32_t
afl::io::MultiplexableStream::Child::getCapabilities()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->getParent()) {
        int n = w->getCapabilities();
        if ((m_flags & DisableWrite) != 0) {
            n &= ~CanWrite;
        }
        return n;
    } else {
        return 0;
    }
}

String_t
afl::io::MultiplexableStream::Child::getName()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->getParent()) {
        return w->getName();
    } else {
        return "<dead>";
    }
}

afl::base::Ref<afl::io::Stream>
afl::io::MultiplexableStream::Child::createChild(uint32_t flags)
{
    return *new Child(m_controlNode, flags | m_flags);
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::MultiplexableStream::Child::createFileMapping(FileSize_t limit)
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->activateChild(this, false)) {
        return w->createFileMapping(limit);
    } else {
        return 0;
    }
}

/************************** MultiplexableStream **************************/

afl::io::MultiplexableStream::MultiplexableStream()
    : m_controlNode(*new ControlNode(this))
{ }

afl::io::MultiplexableStream::~MultiplexableStream()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    m_controlNode->removeParent();
}

afl::base::Ref<afl::io::Stream>
afl::io::MultiplexableStream::createChild(uint32_t flags)
{
    return *new Child(m_controlNode, flags);
}
