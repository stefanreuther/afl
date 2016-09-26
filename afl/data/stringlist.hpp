/**
  *  \file afl/data/stringlist.hpp
  *  \brief Type afl::data::StringList_t
  */
#ifndef AFL_AFL_DATA_STRINGLIST_HPP
#define AFL_AFL_DATA_STRINGLIST_HPP

#include <vector>
#include "afl/string/string.hpp"

namespace afl { namespace data {

    /** String list.
        Convenience typedef for a list of strings. */
    typedef std::vector<String_t> StringList_t;

} }

#endif
