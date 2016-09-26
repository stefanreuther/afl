/**
  *  \file afl/data/namemap.hpp
  *  \brief Class afl::data::NameMap
  */
#ifndef AFL_AFL_DATA_NAMEMAP_HPP
#define AFL_AFL_DATA_NAMEMAP_HPP

#include "afl/base/types.hpp"
#include "afl/string/string.hpp"
#include "afl/data/namequery.hpp"

namespace afl { namespace data {

    /** A Mapping of Names to Indexes.
        Provides a quick mapping in both directions. */
    class NameMap {
     public:
        /** Type for names: string. */
        typedef String_t Name_t;

        /** Type for indexes: integer. */
        typedef uint32_t Index_t;

        /** Index meaning "not found". */
        static const Index_t nil = Index_t(-1);


        /** Constructor. Build blank mapping. */
        NameMap();

        /** Destructor. */
        ~NameMap();


        /** Look up index, given a name.
            \param name Name
            \return Index, nil if not found */
        Index_t getIndexByName(const NameQuery& name) const;

        /** Add a name.
            This allocates a new index and associates it with the given name.
            If the name already exists, the new mapping is created anyway;
            the old index will still report the name but looking up the name will yield the new index.
            \param name Name
            \return Assigned index */
        Index_t add(const Name_t& name);

        /** Add a name unless it is already there.
            If the name already exists, reports its assigned index;
            otherwise, allocates a new index.
            \param name Name
            \return Assigned index */
        Index_t addMaybe(const Name_t& name);

        /** Look up name, given an index.
            \param index Index (address), must exist
            \return Associated name
            \pre index < getNumNames() */
        const Name_t& getNameByIndex(Index_t index) const;

        /** Get number of names.
            \return Number of names */
        Index_t getNumNames() const;

        /** Swap. */
        void swap(NameMap& other);

     private:
        std::vector<Name_t> m_names;
        std::vector<Index_t> m_hashFirst;
        std::vector<Index_t> m_hashNext;
    };

} }

inline const afl::data::NameMap::Name_t&
afl::data::NameMap::getNameByIndex(Index_t index) const
{
    return m_names[index];
}

inline afl::data::NameMap::Index_t
afl::data::NameMap::getNumNames() const
{
    return static_cast<Index_t>(m_names.size());
}

#endif
