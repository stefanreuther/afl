/**
  *  \file afl/base/baseweaklink.hpp
  *  \brief Internal class afl::base::BaseWeakLink
  */
#ifndef AFL_AFL_BASE_BASEWEAKLINK_HPP
#define AFL_AFL_BASE_BASEWEAKLINK_HPP

#include "afl/base/weaktarget.hpp"

namespace afl { namespace base {

    /** Base class for a WeakLink.
        This class is internal for sharing implementation. It should not be used in application code.
        Users use WeakLink instead. */
    class BaseWeakLink {
     public:
        /** Initialize from pointer.
            \param target Pointer (can be null). */
        BaseWeakLink(WeakTarget* target);

        /** Copy constructor.
            \param other Object to copy from. */
        BaseWeakLink(const BaseWeakLink& other);

        /** Destructor. */
        ~BaseWeakLink();

        /** Set to different pointer.
            \param target Pointer (can be null). */
        void reset(WeakTarget* target);

        /** Set to null. */
        void reset();

        /** Get contained pointer.
            \return pointer */
        WeakTarget* get() const;

     private:
        BaseWeakLink& operator=(const BaseWeakLink&);

        friend class WeakTarget;
        void disconnect();
        void init(WeakTarget* target);

        WeakTarget* m_target;
    };

} }

#endif
