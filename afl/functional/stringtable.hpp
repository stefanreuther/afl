/**
  *  \file afl/functional/stringtable.hpp
  *  \brief Typedef afl::functional::StringTable_t
  */
#ifndef AFL_AFL_FUNCTIONAL_STRINGTABLE_HPP
#define AFL_AFL_FUNCTIONAL_STRINGTABLE_HPP

#include "afl/functional/mapping.hpp"
#include "afl/string/string.hpp"
#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace functional {

    namespace detail {
        template<typename T> class StringTable;
    }

    /** Convenience typedef for string tables.
        A string table maps a number (int32_t) to a string.
        This pattern occurs in many contexts, such as error numbers, protocols, etc. */
    typedef Mapping<int32_t,String_t> StringTable_t;

    /** Create string table from array of C strings.
        \param table Array of C strings
        \return StringTable_t. The exact return type is unspecified.
        Pass it to a function that takes a <tt>const StringTable_t&</tt>,
        or bind it to a <tt>const StringTable_t&</tt> local variable. */
    detail::StringTable<const char*> createStringTable(afl::base::Memory<const char*const> table);

    /** Create string table from array of C++ strings.
        \param table Array of C++ strings
        \return StringTable_t. The exact return type is unspecified.
        Pass it to a function that takes a <tt>const StringTable_t&</tt>,
        or bind it to a <tt>const StringTable_t&</tt> local variable. */
    detail::StringTable<String_t> createStringTable(afl::base::Memory<const String_t> table);

} }

template<typename T>
class afl::functional::detail::StringTable : public afl::functional::StringTable_t {
 public:
    StringTable(afl::base::Memory<const T> table)
        : m_table(table)
        { }

    virtual String_t get(int32_t a) const
        {
            if (static_cast<int32_t>(static_cast<size_t>(a)) != a) {
                // Index is not representable
                return String_t();
            } else if (const T* p = m_table.at(a)) {
                return *p;
            } else {
                return String_t();
            }
        }

    virtual bool getFirstKey(int32_t& a) const
        {
            a = 0;
            return m_table.at(0) != 0;
        }

    virtual bool getNextKey(int32_t& a) const
        {
            ++a;
            return m_table.at(a) != 0;
        }

 private:
    const afl::base::Memory<const T> m_table;
};

inline afl::functional::detail::StringTable<const char*>
afl::functional::createStringTable(afl::base::Memory<const char*const> table)
{
    return detail::StringTable<const char*>(table);
}

inline afl::functional::detail::StringTable<String_t>
afl::functional::createStringTable(afl::base::Memory<String_t const> table)
{
    return detail::StringTable<String_t>(table);
}

#endif
