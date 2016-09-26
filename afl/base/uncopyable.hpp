/**
  *  \file afl/base/uncopyable.hpp
  *  \brief Interface afl::base::Uncopyable
  */
#ifndef AFL_AFL_BASE_UNCOPYABLE_HPP
#define AFL_AFL_BASE_UNCOPYABLE_HPP

namespace afl { namespace base {

    /** Base class for uncopyable objects.
        Derive from this class to prevent creation of a standard copy constructor for your class.
        Class instances will therefore not be copyable. */
    class Uncopyable {
     public:
        /** Constructor.
            Required so that this class is not empty. */
        Uncopyable()
            { }

     private:
        Uncopyable(const Uncopyable&);
        Uncopyable& operator=(const Uncopyable&);
    };

} }

#endif
