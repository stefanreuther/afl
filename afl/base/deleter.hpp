/**
  *  \file afl/base/deleter.hpp
  *  \brief Class afl::base::Deleter
  */
#ifndef AFL_AFL_BASE_DELETER_HPP
#define AFL_AFL_BASE_DELETER_HPP

#include <vector>
#include "afl/base/deletable.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace base {

    /** Deletable object tracker.
        A Deleter keeps references to Deletable objects and destroys them when it goes out of scope.
        The idea is to pass it to factory functions who can place all objects they create in it,
        giving the duty of cleaning up to the caller without having to use a heavyweight technology
        such as reference-counting, or being restricted to create a single object such as with std::auto_ptr.

        When building an object structure from local variables or members,
        it also often creates more efficient code when used as
        <pre>
          Deleter d;
          Object1& o1 = d.addNew(new Object1());
          Object2& o2 = d.addNew(new Object2(o1));
          // ...
        </pre>
        instead of the naive
        <pre>
          Object1 o1;
          Object2 o2(o1);
        </pre> */
    class Deleter : public Deletable, private Uncopyable {
     public:
        /** Constructor. Makes empty deleter. */
        Deleter();

        /** Destructor. Will delete all objects it owns. */
        ~Deleter();

        /** Add a new object.
            This stores a newly-allocated object and takes ownership.
            For convenience, it returns a reference for you to use, as in
            <tt>MyObj& o = h.add(new MyObj());</tt>

            If the operation fails, deletes p and rethrows the exception.

            \tparam T type of object. Must be a descendant of Deletable.
            \param p Object. Must not be NULL.
            \return *p */
        template<class T> T& addNew(T* p);

        /** Add a new object without error handling.
            If the operation fails, object p is NOT deleted.
            That is the responsibility of the caller.

            \param p Object. Must not be NULL. */
        void unsafeAddNew(Deletable* p);

        /** Transfer ownership of the objects in this Deleter to another one.
            Objects will not die when this object dies;
            instead, the other one will control their lifetime now.

            If the operation fails, the objects are destroyed; *this is left empty
            in any case.

            \param other Other */
        void moveTo(Deleter& other);

        /** Clear this Deleter, deleting all controlled objects in the process.
            Objects are deleted in reverse order of addition. */
        void clear();

        /** Get number of objects.
            This is an informative value for debugging. */
        size_t size() const;

     private:
        typedef std::vector<Deletable*> List_t;

        List_t m_list;

        void add(Deletable* p);
    };

} }

template<class T>
inline T&
afl::base::Deleter::addNew(T* p)
{
    add(p);
    return *p;
}

inline size_t
afl::base::Deleter::size() const
{
    return m_list.size();
}

#endif
