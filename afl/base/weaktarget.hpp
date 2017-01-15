/**
  *  \file afl/base/weaktarget.hpp
  *  \brief Base class afl::base::WeakTarget
  */
#ifndef AFL_AFL_BASE_WEAKTARGET_HPP
#define AFL_AFL_BASE_WEAKTARGET_HPP

#include <set>

namespace afl { namespace base {

    class BaseWeakLink;

    /** Target for WeakLink.
        A class must derive from WeakTarget if you intend to use WeakLink to point at it.

        Restrictions:
        - WeakTarget/WeakLink do not support multithreading. */
    class WeakTarget {
     public:
        /** Add a WeakLink instance that points here.
            \param link instance */
        void add(BaseWeakLink& link);

        /** Remove a WeakLink instance that pointed here.
            \param link instance */
        void remove(BaseWeakLink& link);

     protected:
        /** Constructor.
            Protected so that you cannot create WeakTarget instances without deriving it. */
        WeakTarget();

        /** Destructor.
            Protected so that you cannot destroy Weakinstances through a WeakTarget pointer, use the derived-class pointer.
            This will reset all WeakLink instances to null. */
        ~WeakTarget();

     private:
        std::set<BaseWeakLink*> m_links;
    };

} }

#endif
