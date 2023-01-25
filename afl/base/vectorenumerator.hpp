/**
  *  \file afl/base/vectorenumerator.hpp
  *  \brief Template class afl::base::VectorEnumerator
  */
#ifndef AFL_AFL_BASE_VECTORENUMERATOR_HPP
#define AFL_AFL_BASE_VECTORENUMERATOR_HPP

#include <vector>
#include "afl/base/enumerator.hpp"

namespace afl { namespace base {

    /** Enumerator for a std::vector.
        Iterates over the elements of the vector.
        @tparam T element type */
    template<typename T>
    class VectorEnumerator : public Enumerator<T> {
     public:
        /** Constructor.
            @param vec Vector. Will be copied. */
        explicit VectorEnumerator(const std::vector<T>& vec);

        /** Constructor.
            Makes an empty VectorEnumerator.
            You may call add() to add more elements. */
        VectorEnumerator();

        /** Implementation of Enumerator::getNextElement.
            @param [out] out Result
            @return true if another element was produced; otherwise, false. */
        bool getNextElement(T& out);

        /** Add another element.
            If iteration has not yet reached the end, getNextElement() will eventually return this element.
            @param ele Element */
        void add(const T& ele);

     private:
        std::vector<T> m_vector;
        size_t m_pos;
    };

} }

template<typename T>
afl::base::VectorEnumerator<T>::VectorEnumerator(const std::vector<T>& vec)
    : m_vector(vec), m_pos(0)
{ }

template<typename T>
inline
afl::base::VectorEnumerator<T>::VectorEnumerator()
    : m_vector(), m_pos(0)
{ }

template<typename T>
bool
afl::base::VectorEnumerator<T>::getNextElement(T& out)
{
    if (m_pos < m_vector.size()) {
        out = m_vector[m_pos++];
        return true;
    } else {
        return false;
    }
}

template<typename T>
inline void
afl::base::VectorEnumerator<T>::add(const T& ele)
{
    m_vector.push_back(ele);
}

#endif
