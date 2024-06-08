/**
  *  \file afl/data/namequery.hpp
  *  \brief Class afl::data::NameQuery
  */
#ifndef AFL_AFL_DATA_NAMEQUERY_HPP
#define AFL_AFL_DATA_NAMEQUERY_HPP

#include "afl/string/string.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace data {

    /** Name Query.
        This stores a query for a name.
        The idea is to "cache" the hash value together with the actual string,
        so that we can pass the pre-hashed value around instead of computing it anew for every lookup step.
        This speeds up workloads that query multiple NameMap; 10..25% have been measured in the PCC2 interpreter.

        NameQuery objects are intended to be temporary.
        They keep references to the objects they were constructed from. */
    class NameQuery {
     public:
        static const size_t HASH_MAX = 64;

        /** Construct from string.
            \param str string */
        NameQuery(const String_t& str);

        /** Construct from string.
            \param str string */
        NameQuery(const char* str);

        /** Construct from tail of a query.
            For example, NameQuery(NameQuery("a.b"), 2) is the same as NameQuery("b").
            \param other other query
            \param skip Skip this many initial characters of \c other */
        NameQuery(const NameQuery& other, size_t skip);

        /** Match string.
            \param str string to compare
            \return true if this NameQuery was constructed from a string equal to \c str */
        bool match(const String_t& str) const;

        /** Match string.
            \param str string to compare
            \return true if this NameQuery was constructed from a string equal to \c str */
        bool match(const char* str) const;

        /** String ordering.
            \param str string to compare
            \return true if this NameQuery was constructed from a string that is lexically less than \c str
            \todo reconsider this method, it does not seem to be needed */
        bool before(const char* str) const;

        /** Compare prefix.
            \param prefix string to compare
            \return true if this NameQuery was constructed from a string that starts with \c prefix. */
        bool startsWith(const char* prefix) const;

        /** Get hash code. */
        size_t getHashCode() const;

     private:
        afl::string::ConstStringMemory_t m_text;
        size_t m_hashCode;
    };

} }


// Match string.
inline bool
afl::data::NameQuery::match(const String_t& str) const
{
    return m_text.equalContent(afl::string::toMemory(str));
}

// Match string.
inline bool
afl::data::NameQuery::match(const char* str) const
{
    return m_text.equalContent(afl::string::toMemory(str));
}

// String ordering.
inline bool
afl::data::NameQuery::before(const char* str) const
{
    return m_text.compareContent(afl::string::toMemory(str)) < 0;
}

// Compare prefix.
inline bool
afl::data::NameQuery::startsWith(const char* prefix) const
{
    afl::string::ConstStringMemory_t mem(afl::string::toMemory(prefix));
    return m_text.subrange(0, mem.size()).equalContent(mem);
}

// Get hash code.
inline size_t
afl::data::NameQuery::getHashCode() const
{
    return m_hashCode;
}

#endif
