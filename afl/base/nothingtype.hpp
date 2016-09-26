/**
  *  \file nothingtype.hpp
  */
#ifndef C2NG_NOTHINGTYPE_HPP
#define C2NG_NOTHINGTYPE_HPP

namespace afl { namespace base {

    /** Unit type to signify "nothing".
        This is used in function signatures by Optional, InlineOptional. */
    enum NothingType {
        Nothing
    };

} }

#endif
