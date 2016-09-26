/**
  *  \file afl/io/multiplexablestream.cpp
  *  \brief Class afl::io::MultiplexableStream
  */

#include <cassert>
#include "afl/io/multiplexablestream.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/io/filemapping.hpp"

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
        \return Stream to perform the requested operation on. Null if operation cannot be performed. */
    Stream* activateChild(Child* ch);

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
    Child(afl::base::Ptr<ControlNode> controlNode);
    virtual ~Child();

    virtual size_t read(Bytes_t m);
    virtual size_t write(ConstBytes_t m);
    virtual void flush();
    virtual void setPos(FileSize_t pos);
    virtual FileSize_t getPos();
    virtual FileSize_t getSize();
    virtual uint32_t getCapabilities();
    virtual String_t getName();
    virtual afl::base::Ptr<Stream> createChild();
    virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit);

 private:
    afl::base::Ptr<ControlNode> m_controlNode;
    FileSize_t m_posIfInactive;

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
afl::io::MultiplexableStream::ControlNode::activateChild(Child* ch)
{
    if (m_parent != 0) {
        if (ch != m_activeChild) {
            // It's not the active child, but we can activate it.
            if (m_activeChild != 0) {
                m_activeChild->m_posIfInactive = m_parent->getPos();
                m_activeChild = 0;
            }
            if (ch != 0) {
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

afl::io::MultiplexableStream::Child::Child(afl::base::Ptr<ControlNode> controlNode)
    : Stream(),
      m_controlNode(controlNode),
      m_posIfInactive(0),
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
    if (Stream* w = m_controlNode->activateChild(this)) {
        return w->read(m);
    } else {
        return 0;
    }
}

size_t
afl::io::MultiplexableStream::Child::write(ConstBytes_t m)
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->activateChild(this)) {
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
    if (Stream* w = m_controlNode->activateChild(this)) {
        w->setPos(pos);
    }
}

afl::io::MultiplexableStream::Child::FileSize_t
afl::io::MultiplexableStream::Child::getPos()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->activateChild(this)) {
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
        return w->getCapabilities();
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

afl::base::Ptr<afl::io::Stream>
afl::io::MultiplexableStream::Child::createChild()
{
    return new Child(m_controlNode);
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::MultiplexableStream::Child::createFileMapping(FileSize_t limit)
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    if (Stream* w = m_controlNode->activateChild(this)) {
        return w->createFileMapping(limit);
    } else {
        return 0;
    }
}

/************************** MultiplexableStream **************************/

afl::io::MultiplexableStream::MultiplexableStream()
    : m_controlNode(new ControlNode(this))
{ }

afl::io::MultiplexableStream::~MultiplexableStream()
{
    afl::sys::MutexGuard g(m_controlNode->m_mutex);
    m_controlNode->removeParent();
}

afl::base::Ptr<afl::io::Stream>
afl::io::MultiplexableStream::createChild()
{
    return new Child(m_controlNode);
}
