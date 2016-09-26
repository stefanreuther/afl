/**
  *  \file afl/data/integerlist.hpp
  *  \brief Type afl::data::IntegerList_t
  */
#ifndef AFL_AFL_DATA_INTEGERLIST_HPP
#define AFL_AFL_DATA_INTEGERLIST_HPP

#include <vector>
#include "afl/base/types.hpp"

namespace afl { namespace data {

    /** Integer list.
        Convenience typedef for a list of integers. */
    typedef std::vector<int32_t> IntegerList_t;

} }


#endif
