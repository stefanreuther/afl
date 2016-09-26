/**
  *  \file afl/base/memory.hpp
  *  \brief Template class afl::base::Memory
  */
#ifndef AFL_AFL_BASE_MEMORY_HPP
#define AFL_AFL_BASE_MEMORY_HPP

#include <algorithm>
#include <cstring>
#include <functional>
#include <vector>
#include "afl/base/types.hpp"
#include "afl/tmp/stripcv.hpp"
#include "afl/tmp/copycv.hpp"

namespace afl { namespace base {

    /** Memory descriptor.
        A Memory object associates a pointer and a size.
        The intention is to make it hard to misplace one or the other.

        It offers safe and unsafe operations.
        - safe operations cannot fail, and will always return valid data.
        - unsafe operations require examination of the parameters */
    template<typename T>
    class Memory {
        template<typename U> friend class Memory;

     public:
        /** Construct empty.
            This is the equivalent of a null pointer. */
        Memory();

        /** Construct from array.
            The descriptor refers to the array.
            \param N array size
            \param array data */
        template<size_t N>
        Memory(T (&array)[N]);

        /** Construct from different type.
            \param U other type. Must be equivalent to this descriptor's type, but can have more CV qualifiers.
            \param other other descriptor */
        template<typename U>
        Memory(Memory<U> other);

        /** Construct from vector.
            The descriptor will refer to the vector content.
            \param vec vector */
        Memory(std::vector<typename afl::tmp::StripCV<T>::Type>& vec);

        /** Construct from constant vector.
            The descriptor will refer to the vector content.
            \param vec vector */
        Memory(const std::vector<typename afl::tmp::StripCV<T>::Type>& vec);

        /** Check emptiness.
            \return true iff this descriptor is empty (default constructed, size zero). */
        bool empty() const;

        /** Get number of elements.
            \return number of elements */
        size_t size() const;

        /** Get subrange.
            \param pos First index. If this value is out of range, the result will be empty.
            \param len Number of elements. If this value is too large, the result will be truncated accordingly.
            \return subrange */
        Memory subrange(size_t pos, size_t len = size_t(-1)) const;

        /** Chop off part at beginning.
            Updates this descriptor by removing up to %amount elements,
            and returns a descriptor to the removed elements.

            This function is intended for suballocating a descriptor.
            \param amount Number of elements to chop off
            \return descriptor to the removed elements */
        Memory split(size_t amount);

        /** Chop off part at beginning and update count.
            Updates this descriptor by removing up to %amount elements,
            reduces %amount by the number of removed elements,
            and returns a descriptor to the removed elements.

            This function is intended for reblocking data from multiple descriptors.
            It can be used as a replacement to code like
            <pre>
              Memory x = m.split(n);
              n -= x.size();
            </pre>
            and handles the case that %amount is a value other than a %size_t
            (including half-word, double-word, and signed values).

            \param Size_t size type, any integer type
            \param amount [in/out] On input, number of elements to chop off.
            On output, number of "missing" elements (0 if all requested elements were found).
            \return descriptor to the removed elements */
        template<typename Size_t>
        Memory splitUpdate(Size_t& amount);

        /** Reduce size by chopping off part at end.
            Limits the size to the given value.
            If the descriptor already contains fewer elements, nothing happens.
            \param len New maximum size
            \return *this */
        Memory& trim(size_t len);

        /** Merge two descriptors.
            If the other descriptor describes memory directly past the end of this descriptor,
            updates this one to include the other one.

            This is the inverse operation to split(); after <code>a = b.split(n);</code>
            use <code>a.merge(b);</code> to merge them again.

            An empty descriptor on either side can be successful merged to any other descriptor.

            \param end other descriptor
            \retval true merge successful; *this has been updated
            \retval false merge not possible because buffers are not adjacent; *this unchanged */
        bool merge(const Memory& end);

        /** Reset. Make this descriptor empty. */
        void reset();

        /** Element access.
            \param n Element index
            \return pointer to n'th element, 0 if that does not exist */
        T* at(size_t n) const;

        /** Remove first element.
            If there is at least one element in this memory buffer,
            removes that (by shrinking the memory buffer's size) and returns a pointer to it.
            If there is no element remaining, returns 0.

            This is treated as a safe operation because it's intended to be used in a
            "if (T* p = eat())" pattern.
            \return pointer to element, 0 if that does not exist */
        T* eat();

        /** Remove first elements.
            If there are N bytes in the memory buffer,
            removes them (by shrinking the memory buffer's size) and returns a typed pointer to those.

            If there are fewer than N bytes remaining, returns 0.
            (This means a partial element or no element at all remaining.)

            This is treated as a safe operation because it's intended to be used in a
            "if (T (*p)[N] = eatN<N>())" pattern.

            \param N number of bytes to remove
            \return pointer to array of N bytes, 0 if that could not be extracted */
        template<size_t N>
        T (*eatN())[N];

        /** Last element access.
            \param n Element index (0=last, 1=second-last, etc.)
            \return pointer to desired element, 0 if that does not exist */
        T* atEnd(size_t n) const;

        /** Remove last elements.
            Removes the last \c n elements by reducing the size by \c n.
            \return *this */
        Memory<T>& removeEnd(size_t n);

        /** Compare for equality.
            \param other Other memory block
            \return true iff both objects describe the same memory, or both are null */
        bool operator==(Memory other) const;

        /** Compare for inequality.
            \param other Other memory block
            \return false iff both objects describe the same memory, or both are null */
        bool operator!=(Memory other) const;

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
            If sizes of both blocks differ, copies just as much as will fit.
            \param other Other memory block
            \return descriptor to copied memory (a subrange of this) */
        Memory copyFrom(const Memory<const T>& other) const;

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
            \return descriptor for the bytes covered by this descriptor */
        Memory<typename afl::tmp::CopyCV<T, uint8_t>::Type> toBytes() const;



        /*
         *  Unsafe operations
         */

        /** Create descriptor from pointer and size.
            This is an unsafe operation because you have to make sure that pointer and size match.
            \return new descriptor */
        static Memory unsafeCreate(T* ptr, size_t size);

        /** Get pointer to data.
            This is an unsafe operation because you have to make sure that the returned pointer
            is used correctly and paired with this descriptor's size.
            \return pointer; can be null if the descriptor was empty (in this case, size() == 0). */
        T* unsafeData() const;

     private:
        Memory(T* ptr, size_t size);
        
        T* m_ptr;
        size_t m_size;
    };



    /** Descriptor for raw bytes. */
    typedef Memory<uint8_t> Bytes_t;

    /** Descriptor for constant raw bytes. */
    typedef Memory<const uint8_t> ConstBytes_t;

    /** Create raw byte descriptor from an object.
        \param obj the object
        \return descriptor describing the object */
    template<typename T> Bytes_t fromObject(T& obj);

    /** Create raw byte descriptor from a constant object.
        \param obj the object
        \return descriptor describing the object */
    template<typename T> ConstBytes_t fromObject(const T& obj);


} }

/***************************** Implementation ****************************/

namespace afl { namespace base { namespace detail {
    template<typename T>
    struct MemoryGenericTraits {
        static bool equal(const T* a, const T* b, size_t count);
        static int  compare(const T* a, const T* b, size_t count);
        static void fill(T* a, const T& val, size_t count);
        static void copy(T* dest, const T* src, size_t count);
        static size_t find(const T* a, size_t count, const T& elem);
    };

    template<typename T>
    struct MemoryPodTraits {
        static bool equal(const T* a, const T* b, size_t count)
            { return MemoryGenericTraits<T>::equal(a, b, count); }
        static int  compare(const T* a, const T* b, size_t count)
            { return MemoryGenericTraits<T>::compare(a, b, count); }
        static void fill(T* a, const T& val, size_t count)
            { return MemoryGenericTraits<T>::fill(a, val, count); }
        static void copy(T* dest, const T* src, size_t count)
            { std::memmove(dest, src, count * sizeof(T)); }
        static size_t find(const T* a, size_t count, const T& elem)
            { return MemoryGenericTraits<T>::find(a, count, elem); }
    };

    struct MemoryCharTraits {
        static bool equal(const void* a, const void* b, size_t count)
            { return compare(a, b, count) == 0; }
        static int  compare(const void* a, const void* b, size_t count)
            { return std::memcmp(a, b, count); }
        static void fill(void* a, const int val, size_t count)
            { std::memset(a, val, count); }
        static void copy(void* dest, const void* src, size_t count)
            { std::memmove(dest, src, count); }
        static size_t find(const void* a, size_t count, int n)
            {
                const void* result = std::memchr(a, n, count);
                return result == 0
                    ? count
                    : static_cast<const char*>(result) - static_cast<const char*>(a);
            }
    };

    template<typename T>
    struct MemoryTraits : public MemoryGenericTraits<T> { };

    template<> struct MemoryTraits<char>          : public MemoryCharTraits { };
    template<> struct MemoryTraits<unsigned char> : public MemoryCharTraits { };
    template<> struct MemoryTraits<signed char>   : public MemoryCharTraits { };

    template<> struct MemoryTraits<bool>          : public MemoryPodTraits<bool> { };
    template<> struct MemoryTraits<wchar_t>       : public MemoryPodTraits<wchar_t> { };
    template<> struct MemoryTraits<float>         : public MemoryPodTraits<float> { };
    template<> struct MemoryTraits<double>        : public MemoryPodTraits<double> { };

    template<> struct MemoryTraits<uint16_t>      : public MemoryPodTraits<uint16_t> { };
    template<> struct MemoryTraits<uint32_t>      : public MemoryPodTraits<uint32_t> { };
    template<> struct MemoryTraits<uint64_t>      : public MemoryPodTraits<uint64_t> { };
    template<> struct MemoryTraits<int16_t>       : public MemoryPodTraits<int16_t> { };
    template<> struct MemoryTraits<int32_t>       : public MemoryPodTraits<int32_t> { };
    template<> struct MemoryTraits<int64_t>       : public MemoryPodTraits<int64_t> { };

} } }

template<typename T>
bool
afl::base::detail::MemoryGenericTraits<T>::equal(const T* a, const T* b, size_t count)
{
    return std::equal(a, a + count, b);
}

template<typename T>
int
afl::base::detail::MemoryGenericTraits<T>::compare(const T* a, const T* b, size_t count)
{
    while (count > 0 && *a == *b) {
        ++a, ++b, --count;
    }
    if (count == 0) {
        return 0;
    } else if (*a < *b) {
        return -1;
    } else {
        return +1;
    }
}

template<typename T>
void
afl::base::detail::MemoryGenericTraits<T>::fill(T* a, const T& val, size_t count)
{
    std::fill_n(a, count, val);
}

template<typename T>
void
afl::base::detail::MemoryGenericTraits<T>::copy(T* dest, const T* src, size_t count)
{
    std::less<const T*> cmp;
    if (src == dest) {
        // nothing to do
    } else if (cmp(src, dest) && cmp(dest, src + count)) {
        // overlap: src_begin < dest < src_end < dest_end
        std::copy_backward(src, src + count, dest + count);
    } else {
        // no overlap
        std::copy(src, src + count, dest);
    }
}

template<typename T>
size_t
afl::base::detail::MemoryGenericTraits<T>::find(const T* a, size_t count, const T& val)
{
    return std::find(a, a+count, val) - a;
}


/***************************** Implementation ****************************/

template<typename T>
inline
afl::base::Memory<T>::Memory()
    : m_ptr(0),
      m_size(0)
{ }

template<typename T>
template<size_t N>
inline
afl::base::Memory<T>::Memory(T (&array)[N])
    : m_ptr(array),
      m_size(N)
{ }

template<typename T>
template<typename U>
inline
afl::base::Memory<T>::Memory(Memory<U> other)
    : m_ptr(other.m_ptr),
      m_size(other.m_size)
{
    // The compiler will have verified that other.m_ptr can be assigned to m_ptr.
    // This is acceptable if other.m_ptr is, say, 'char*', and m_ptr is 'const char*'.
    // However, the compiler will also allow assigning 'Derived*' to 'Base*', which we do not want.
    // We must therefore make sure the base types are the same.
    // The simplest is to compare 'T**' and 'U**', which will fail to compile if they are not equal.
    (void) (static_cast<typename afl::tmp::StripCV<T>::Type**>(0) == static_cast<typename afl::tmp::StripCV<U>::Type**>(0));
}

template<typename T>
inline
afl::base::Memory<T>::Memory(T* ptr, size_t size)
    : m_ptr(ptr),
      m_size(size)
{ }

// This constructor is intended to be used for mutable vectors / mutable Memory.
// Still, it takes part in overload resolution for const Memory, and some compilers will then fail overload resolution
// for trying to instantiate a std::vector<const T>. Thus, strip the cv-qualifiers.
template<typename T>
inline
afl::base::Memory<T>::Memory(std::vector<typename afl::tmp::StripCV<T>::Type>& vec)
    : m_ptr(vec.empty() ? 0 : &vec[0]),
      m_size(vec.empty() ? 0 : vec.size())
{ }

template<typename T>
inline
afl::base::Memory<T>::Memory(const std::vector<typename afl::tmp::StripCV<T>::Type>& vec)
    : m_ptr(vec.empty() ? 0 : &vec[0]),
      m_size(vec.empty() ? 0 : vec.size())
{ }

template<typename T>
inline bool
afl::base::Memory<T>::empty() const
{
    return m_size == 0;
}

template<typename T>
inline size_t
afl::base::Memory<T>::size() const
{
    return m_size;
}

template<typename T>
afl::base::Memory<T>
afl::base::Memory<T>::subrange(size_t pos, size_t len) const
{
    size_t realPos  = std::min(pos, m_size);
    size_t realSize = std::min(len, m_size - realPos);
    if (realSize != 0) {
        return Memory(&m_ptr[realPos], realSize);
    } else {
        return Memory();
    }
}

template<typename T>
afl::base::Memory<T>
afl::base::Memory<T>::split(size_t amount)
{
    size_t realSize = std::min(amount, m_size);

    Memory result(m_ptr, realSize);
    if (realSize != 0) {
        m_ptr += realSize;
        m_size -= realSize;
    }

    return result;
}

template<typename T>
template<typename Size_t>
afl::base::Memory<T>
afl::base::Memory<T>::splitUpdate(Size_t& amount)
{
    // We want this comparison to be correct and warning-free for all sorts of different types.
    // - the first check weeds out negative values.
    // - the second check checks that we can successfully convert the value to size_t and back.
    //   If we cannot, it does obviously not fit into a size_t (e.g. Size_t = uint64_t and size_t being 32 bits.)
    // - finally, we can compare sizes normally.
    size_t realSize = (amount <= 0 ? 0
                       : Size_t(size_t(amount)) != amount ? m_size
                       : std::min(size_t(amount), m_size));

    Memory result(m_ptr, realSize);
    if (realSize != 0) {
        m_ptr += realSize;
        m_size -= realSize;
        amount = static_cast<Size_t>(amount - realSize);
    }

    return result;
}

template<typename T>
inline afl::base::Memory<T>&
afl::base::Memory<T>::trim(size_t len)
{
    m_size = std::min(m_size, len);
    return *this;
}

template<typename T>
bool
afl::base::Memory<T>::merge(const Memory& end)
{
    if (end.m_size == 0) {
        // We can merge empty into anything
        return true;
    } else if (m_size == 0) {
        // We can merge anything into empty
        *this = end;
        return true;
    } else if (m_ptr + m_size == end.m_ptr) {
        // Merge adjacent
        m_size += end.m_size;
        return true;
    } else {
        // Not mergeable
        return false;
    }
}

template<typename T>
inline void
afl::base::Memory<T>::reset()
{
    m_size = 0;
    m_ptr = 0;
}

template<typename T>
inline T*
afl::base::Memory<T>::at(size_t n) const
{
    if (n < m_size) {
        return &m_ptr[n];
    } else {
        return 0;
    }
}

template<typename T>
inline T*
afl::base::Memory<T>::eat()
{
    T* result;
    if (m_size == 0) {
        result = 0;
    } else {
        result = m_ptr;
        ++m_ptr;
        --m_size;
    }
    return result;
}

template<typename T>
template<size_t N>
inline T
(*afl::base::Memory<T>::eatN())[N]
{
    typedef typename afl::tmp::CopyCV<T, void>::Type Void_t;
    typedef T Result_t[N];
    Result_t* result;
    if (m_size < N) {
        result = 0;
    } else {
        result = static_cast<Result_t*>(static_cast<Void_t*>(m_ptr));
        m_ptr += N;
        m_size -= N;
    }
    return result;
}

// Last element access.
template<typename T>
T*
afl::base::Memory<T>::atEnd(size_t n) const
{
    if (n < m_size) {
        return &m_ptr[m_size-n-1];
    } else {
        return 0;
    }
}

// Remove last elements.
template<typename T>
afl::base::Memory<T>&
afl::base::Memory<T>::removeEnd(size_t n)
{
    if (n < m_size) {
        m_size -= n;
    } else {
        m_size = 0;
    }
    return *this;
}

template<typename T>
inline bool
afl::base::Memory<T>::operator==(Memory other) const
{
    return m_size == other.m_size
        && (m_size == 0 || m_ptr == other.m_ptr);
}

template<typename T>
inline bool
afl::base::Memory<T>::operator!=(Memory other) const
{
    return !this->operator==(other);
}

template<typename T>
inline void
afl::base::Memory<T>::fill(const T& elem) const
{
    if (m_size != 0) {
        detail::MemoryTraits<T>::fill(m_ptr, elem, m_size);
    }
}

template<typename T>
bool
afl::base::Memory<T>::equalContent(const Memory<const T>& other) const
{
    return m_size == other.m_size
        && (m_size == 0
            || detail::MemoryTraits<T>::equal(m_ptr, other.m_ptr, m_size));
}

template<typename T>
int
afl::base::Memory<T>::compareContent(const Memory<const T>& other) const
{
    size_t amountToCompare = std::min(m_size, other.m_size);
    if (amountToCompare != 0) {
        if (int delta = detail::MemoryTraits<T>::compare(m_ptr, other.m_ptr, amountToCompare)) {
            return delta;
        }
    }

    if (m_size < other.m_size) {
        return -1;
    } else {
        return +1;
    }
}

template<typename T>
afl::base::Memory<T>
afl::base::Memory<T>::copyFrom(const Memory<const T>& other) const
{
    size_t amountToCopy = std::min(m_size, other.m_size);
    if (amountToCopy != 0) {
        detail::MemoryTraits<T>::copy(m_ptr, other.m_ptr, amountToCopy);
    }
    return Memory(m_ptr, amountToCopy);
}

// Find value (memchr).
template<typename T>
size_t
afl::base::Memory<T>::find(const T& elem) const
{
    return detail::MemoryTraits<T>::find(m_ptr, m_size, elem);
}

// Find value from a list (strpbrk).
template<typename T>
size_t
afl::base::Memory<T>::findFirstOf(Memory<const T> elems) const
{
    if (elems.empty()) {
        return m_size;
    } else {
        size_t i = 0;
        while (i < m_size && elems.find(m_ptr[i]) == elems.m_size) {
            ++i;
        }
        return i;
    }
}

template<typename T>
inline afl::base::Memory<typename afl::tmp::CopyCV<T, uint8_t>::Type>
afl::base::Memory<T>::toBytes() const
{
    typedef typename afl::tmp::CopyCV<T, uint8_t>::Type Bytes_t;
    typedef typename afl::tmp::CopyCV<T, void>::Type    Void_t;
    return Memory<Bytes_t>(static_cast<Bytes_t*>(static_cast<Void_t*>(m_ptr)), m_size * sizeof(T));
}

template<typename T>
inline afl::base::Memory<T>
afl::base::Memory<T>::unsafeCreate(T* ptr, size_t size)
{
    return Memory(ptr, size);
}

template<typename T>
inline T*
afl::base::Memory<T>::unsafeData() const
{
    return m_ptr;
}

template<typename T>
inline afl::base::Bytes_t
afl::base::fromObject(T& obj)
{
    return Bytes_t::unsafeCreate(static_cast<uint8_t*>(static_cast<void*>(&obj)), sizeof(obj));
}

template<typename T>
inline afl::base::ConstBytes_t
afl::base::fromObject(const T& obj)
{
    return ConstBytes_t::unsafeCreate(static_cast<const uint8_t*>(static_cast<const void*>(&obj)), sizeof(obj));
}

#endif
