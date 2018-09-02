/**
  *  \file afl/base/growablememory.hpp
  *  \brief Template class afl::base::GrowableMemory
  */
#ifndef AFL_AFL_BASE_GROWABLEMEMORY_HPP
#define AFL_AFL_BASE_GROWABLEMEMORY_HPP

#include <stdexcept>
#include "afl/base/memory.hpp"
#include "afl/tmp/stripcv.hpp"

namespace afl { namespace base {

    /** Growable memory buffer.
        This buffer is an easy, simple, fast container for small objects like chars.
        It has a more convenient interface than std::vector for bulk operations,
        and integrates nicely with Memory<T>.

        It will most likely perform very bad for items which are not PODs. */
    template<typename T>
    class GrowableMemory {
        static const size_t MINIMUM_CAPACITY = 256 / sizeof(T);
     public:
        /** Constructor.
            Makes an empty container. */
        GrowableMemory();

        /** Copy constructor. */
        GrowableMemory(const GrowableMemory& other);

        /** Destructor. */
        ~GrowableMemory();


        /*
         *  Memory operations
         */

        /** Check emptiness.
            \return true iff this descriptor is empty (default constructed, size zero). */
        bool empty() const;

        /** Get number of elements.
            \return number of elements */
        size_t size() const;

        /** Get current capacity.
            \return capacity */
        size_t getCapacity() const;

        /** Get subrange.
            \param pos First index. If this value is out of range, the result will be empty.
            \param len Number of elements. If this value is too large, the result will be truncated accordingly.
            \return subrange */
        Memory<T> subrange(size_t pos, size_t len = size_t(-1)) const;

        /** Reduce size by chopping off part at end.
            Limits the size to the given value.
            If the container already contains fewer elements, nothing happens.

            This function does not reallocate.
            It only reduces the perceived size, it does not free unused memory.

            \param len New maximum size
            \return *this */
        GrowableMemory& trim(size_t len);

        /** Reset.
            Make this container empty.

            This function does not reallocate.
            It only reduces the perceived size, it does not free unused memory. */
        void reset();

        /** Clear.
            This is an alternative name for reset() to correspond with STL function names. */
        void clear();

        /** Element access.
            \param n Element index
            \return pointer to n'th element, 0 if that does not exist */
        T* at(size_t n) const;

        /** Last element access.
            \param n Element index (0=last, 1=second-last, etc.)
            \return pointer to desired element, 0 if that does not exist */
        T* atEnd(size_t n) const;

        /** Remove last elements.
            Removes the last \c n elements by reducing the size by \c n.
            \return *this */
        GrowableMemory<T>& removeEnd(size_t n);

        /** Fill with single value (memset).
            \param elem Value to fill with */
        void fill(const T& elem) const;

        /** Compare content for equality (memcmp).
            This function only checks for equality and is therefore faster than compareContent.
            \param other Other memory block
            \return true iff the content of both objects are the same, or both are null */
        bool equalContent(const Memory<const T>& other) const;

        /** Compare content (memcmp).
            This function provides a lexicographical ordering.
            \param other Other memory block
            \return negative if other comes before this, 0 if equal, positive if other comes after this */
        int compareContent(const Memory<const T>& other) const;

        /** Copy from another block (memmove).
            This function models the equivalent function of Memory<T>:
            If sizes of both blocks differ, copies just as much as will fit.

            To make this GrowableMemory contain a copy of a Memory, do
            <tt>growable.reset(); growable.append(mem);</tt>.

            \param other Other memory block
            \return descriptor to copied memory (a subrange of this) */
        Memory<T> copyFrom(const Memory<const T>& other) const;

        /** Find value (memchr).
            Returns the number of elements preceding the first instance of the given element.
            In particular, if the element was not found, returns size().
            Idioms:
            - trim(find(N)): initial sequence consisting of elements not equal to \c N
            - subrange(find(N)): sequence starting at the first instance of \c N
            \param elem Element to find
            \return Number of elements preceding the first instance of elem. */
        size_t find(const T& elem) const;

        /** Find value from a list (strpbrk).
            Returns the number of elements preceding the first instance of an element from the given set.
            In particular, if no such element was found, returns size().
            Idioms:
            - trim(find(ns)): initial sequence consisting of elements not from \c ns
            - subrange(find(ns)): sequence starting at the first instance any of \c ns
            \param elems Buffer with elements to find
            \return Number of elements preceding the first instance of a matching element. */
        size_t findFirstOf(Memory<const T> elems) const;

        /** Convert to byte array.
            \return descriptor for the bytes covered by this container */
        Memory<typename afl::tmp::CopyCV<T, uint8_t>::Type> toBytes() const;

        /** Get pointer to data.
            This is an unsafe operation because you have to make sure that the returned pointer
            is used correctly and paired with this container's size.
            \return pointer; can be null if the container was empty (in this case, size() == 0). */
        T* unsafeData() const;


        /*
         *  GrowableMemory operations
         */

        /** Ensure a minimum capacity.
            This may reallocate the container and invalidate all pointers and descriptors obtained from it.
            If the capacity is already big enough, nothing happens.
            \param newCapacity new minimum capacity */
        void reserve(size_t newCapacity);

        /** Ensure a minimum size.
            This may reallocate the container and invalidate all pointers and descriptors obtained from it.
            If this container contains fewer elements, its size is modified to be able to hold at least the specified amount.
            If the container already contains more than that, nothing happens.
            \param newSize new minimum size */
        void ensureSize(size_t newSize);

        /** Set size.
            If the buffer is smaller than the given size, makes it larger (ensureSize()),
            which may reallocate the container and invalidate all pointers and descriptors obtained from it.
            If the buffer is larger than the given size, reduces its size (trim()) without reallocation.
            \param newSize new size */
        void resize(size_t newSize);

        /** Append array of items.
            This may reallocate the container and invalidate all pointers and descriptors obtained from it.
            \param data data to append */
        void append(Memory<const T> data);

        /** Append a single item.
            This may reallocate the container and invalidate all pointers and descriptors obtained from it.
            \param what item to append */
        void append(T what);

        /** Append item, replicated.
            This may reallocate the container and invalidate all pointers and descriptors obtained from it.
            \param what  item to append
            \param count number of times to append that item.
            \return Handle to appended elements */
        Memory<T> appendN(T what, size_t count);

        /** Swap containers.
            \param other other container */
        void swap(GrowableMemory<T>& other);

        /** Convert to Memory (explicit).
            Obtains a Memory that describes the current content of this GrowableMemory. */
        Memory<T> toMemory() const;

        /** Convert to Memory (implicit).
            This signature allows using the GrowableMemory wherever a Memory is required.

            There are two signatures, one producing a 'mutable T', one producing a 'const T'.
            This is required to allow GrowableMemory<T> => Memory<const T> conversions.

            The 'mutable T' signature must be written with StripCV to make both signatures different if 'T' already is const.
            It will fail to instantiate if a GrowableMemory<const T> => Memory<T> conversion is attempted. */
        operator Memory<typename afl::tmp::StripCV<T>::Type>() const;
        operator Memory<const T>() const;

        /** Iterator access: beginning. */
        T* begin() const;

        /** Iterator access: end. */
        T* end() const;

     private:
        T* m_data;
        size_t m_capacity;
        size_t m_size;

        T* ensureCapacityInternal(size_t n);
    };

    /** Descriptor for growable raw bytes. */
    typedef GrowableMemory<uint8_t> GrowableBytes_t;

} }

template<typename T>
const size_t afl::base::GrowableMemory<T>::MINIMUM_CAPACITY;

// Constructor.
template<typename T>
afl::base::GrowableMemory<T>::GrowableMemory()
    : m_data(0),
      m_capacity(0),
      m_size(0)
{ }

// Copy constructor.
template<typename T>
afl::base::GrowableMemory<T>::GrowableMemory(const GrowableMemory& other)
    : m_data(0),
      m_capacity(0),
      m_size(0)
{
    append(other);
}

// Destructor.
template<typename T>
afl::base::GrowableMemory<T>::~GrowableMemory()
{
    delete[] m_data;
}

// Check emptiness.
template<typename T>
inline bool
afl::base::GrowableMemory<T>::empty() const
{
    return m_size == 0;
}

// Get number of elements.
template<typename T>
inline size_t
afl::base::GrowableMemory<T>::size() const
{
    return m_size;
}

// Get current capacity.
template<typename T>
inline size_t
afl::base::GrowableMemory<T>::getCapacity() const
{
    return m_capacity;
}

// Get subrange.
template<typename T>
afl::base::Memory<T>
afl::base::GrowableMemory<T>::subrange(size_t pos, size_t len) const
{
    return toMemory().subrange(pos, len);
}

// Reduce size by chopping off part at end.
template<typename T>
inline afl::base::GrowableMemory<T>&
afl::base::GrowableMemory<T>::trim(size_t len)
{
    m_size = std::min(m_size, len);
    return *this;
}

// Reset.
template<typename T>
inline void
afl::base::GrowableMemory<T>::reset()
{
    m_size = 0;
}

// Clear.
template<typename T>
inline void
afl::base::GrowableMemory<T>::clear()
{
    reset();
}

// Element access.
template<typename T>
inline T*
afl::base::GrowableMemory<T>::at(size_t n) const
{
    return toMemory().at(n);
}

// Last element access.
template<typename T>
inline T*
afl::base::GrowableMemory<T>::atEnd(size_t n) const
{
    return toMemory().atEnd(n);
}

// Remove last elements.
template<typename T>
afl::base::GrowableMemory<T>&
afl::base::GrowableMemory<T>::removeEnd(size_t n)
{
    if (n < m_size) {
        m_size -= n;
    } else {
        m_size = 0;
    }
    return *this;
}

// Fill with single value (memset).
template<typename T>
inline void
afl::base::GrowableMemory<T>::fill(const T& elem) const
{
    toMemory().fill(elem);
}

// Compare content for equality (memcmp).
template<typename T>
inline bool
afl::base::GrowableMemory<T>::equalContent(const Memory<const T>& other) const
{
    return toMemory().equalContent(other);
}

// Compare content (memcmp).
template<typename T>
inline int
afl::base::GrowableMemory<T>::compareContent(const Memory<const T>& other) const
{
    return toMemory().compareContent(other);
}

// Copy from another block (memmove).
template<typename T>
afl::base::Memory<T>
afl::base::GrowableMemory<T>::copyFrom(const Memory<const T>& other) const
{
    return toMemory().copyFrom(other);
}

// Find value (memchr).
template<typename T>
inline size_t
afl::base::GrowableMemory<T>::find(const T& elem) const
{
    return toMemory().find(elem);
}

// Find value from a list (strpbrk).
template<typename T>
inline size_t
afl::base::GrowableMemory<T>::findFirstOf(Memory<const T> elems) const
{
    return toMemory().findFirstOf(elems);
}

// Convert to byte array.
template<typename T>
inline afl::base::Memory<typename afl::tmp::CopyCV<T, uint8_t>::Type>
afl::base::GrowableMemory<T>::toBytes() const
{
    return toMemory().toBytes();
}

// Get pointer to data.
template<typename T>
inline T*
afl::base::GrowableMemory<T>::unsafeData() const
{
    return m_data;
}

// Ensure a minimum capacity.
template<typename T>
void
afl::base::GrowableMemory<T>::reserve(size_t newCapacity)
{
    delete[] ensureCapacityInternal(newCapacity);
}

// Ensure a minimum size.
template<typename T>
void
afl::base::GrowableMemory<T>::ensureSize(size_t newSize)
{
    if (newSize > m_size) {
        reserve(newSize);
        m_size = newSize;
    }
}

// Set size.
template<typename T>
void
afl::base::GrowableMemory<T>::resize(size_t newSize)
{
    if (newSize > m_size) {
        ensureSize(newSize);
    } else {
        trim(newSize);
    }
}

// Append array of items.
template<typename T>
void
afl::base::GrowableMemory<T>::append(Memory<const T> data)
{
    if (!data.empty()) {
        size_t newSize = m_size + data.size();
        if (newSize < m_size) {
            // overflow!
            throw std::bad_alloc();
        }

        // data could be pointing into our memory, e.g. if someone is doing 'x.append(x);'.
        // We therefore keep data around until after the copy.
        T* toDelete = ensureCapacityInternal(newSize);
        try {
            detail::MemoryTraits<T>::copy(&m_data[m_size], data.unsafeData(), data.size());
            m_size += data.size();
            delete[] toDelete;
        }
        catch (...) {
            delete[] toDelete;
            throw;
        }
    }
}

// Append a single item.
template<typename T>
void
afl::base::GrowableMemory<T>::append(T what)
{
    appendN(what, 1U);
}

// Append item, replicated.
template<typename T>
afl::base::Memory<T>
afl::base::GrowableMemory<T>::appendN(T what, size_t count)
{
    if (count != 0) {
        if (m_size + count < m_size) {
            throw std::bad_alloc();
        }
        reserve(m_size + count);
        detail::MemoryTraits<T>::fill(&m_data[m_size], what, count);
        m_size += count;
        return Memory<T>::unsafeCreate(&m_data[m_size - count], count);
    } else {
        return Memory<T>();
    }
}

// Swap containers.
template<typename T>
void
afl::base::GrowableMemory<T>::swap(GrowableMemory<T>& other)
{
    std::swap(m_data, other.m_data);
    std::swap(m_size, other.m_size);
    std::swap(m_capacity, other.m_capacity);
}

// Convert to Memory (explicit).
template<typename T>
inline afl::base::Memory<T>
afl::base::GrowableMemory<T>::toMemory() const
{
    return Memory<T>::unsafeCreate(m_data, m_size);
}

template<typename T>
afl::base::GrowableMemory<T>::operator Memory<typename afl::tmp::StripCV<T>::Type>() const
{
    return toMemory();
}

template<typename T>
afl::base::GrowableMemory<T>::operator Memory<const T>() const
{
    return toMemory();
}

// Ensure a minimum capacity.
template<typename T>
T*
afl::base::GrowableMemory<T>::ensureCapacityInternal(size_t newCapacity)
{
    T* result = 0;
    if (newCapacity >= m_capacity) {
        // Try exponential growth.
        newCapacity = std::max(newCapacity, m_size*2);
        newCapacity = std::max(newCapacity, MINIMUM_CAPACITY);

        // Reallocate
        T* n = new T[newCapacity];  // may throw
        try {
            if (m_size != 0) {
                detail::MemoryTraits<T>::copy(n, m_data, m_size);
            }
            result = m_data;
            m_data = n;
            m_capacity = newCapacity;
        }
        catch(...) {            // can only come from copy
            delete[] n;
            throw;
        }
    }
    return result;
}

/** Iterator access: beginning. */
template<typename T>
inline T*
afl::base::GrowableMemory<T>::begin() const
{
    return m_data;
}

/** Iterator access: end. */
template<typename T>
inline T*
afl::base::GrowableMemory<T>::end() const
{
    return m_data + m_size;
}

#endif
