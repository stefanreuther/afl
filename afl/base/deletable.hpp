/**
  *  \file afl/base/deletable.hpp
  *  \brief Interface afl::base::Deletable
  */
#ifndef AFL_AFL_BASE_DELETABLE_HPP
#define AFL_AFL_BASE_DELETABLE_HPP

namespace afl { namespace base {

    /** Deletable interface.
        This class makes sure that everything derived from this class has a virtual destructor,
        and therefore can be safely deleted polymorphically.

        It is not usually good style to operate with generic pointers to Deletable
        and dynamic_cast them as needed, but sometimes it is still useful.
        In any case it's good style to derive your interfaces from this
        to avoid forgetting the virtual destructor. */
    class Deletable {
     public:
        /** Virtual destructor. */
        virtual ~Deletable() = 0;
    };

} }


inline
afl::base::Deletable::~Deletable()
{ }

#endif
