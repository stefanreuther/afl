/**
  *  \file test/afl/container/memberiteratortest.cpp
  *  \brief Test for afl::container::MemberIterator
  */

#include "afl/container/memberiterator.hpp"

#include <map>
#include "afl/container/dereferencingiterator.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.container.MemberIterator", a)
{
    std::map<int,int*> imap;
    imap[1] = new int(17);
    imap[17] = new int(42);
    imap[42] = new int(3);

    // Member iterator
    {
        typedef afl::container::MemberIterator<std::map<int,int*>::iterator,
                                               const int,
                                               &std::pair<const int,int*>::first> mite;

        mite i = mite(imap.begin());
        a.checkEqual("mem 0", *i, 1);  a.check("mem 0 end", i != mite(imap.end())); ++i;
        a.checkEqual("mem 1", *i, 17); a.check("mem 1 end", i != mite(imap.end())); ++i;
        a.checkEqual("mem 2", *i, 42); a.check("mem 2 end", i != mite(imap.end())); ++i;
        a.check("mem 3 end", i == mite(imap.end()));
    }

    // Member + Pointer iterator
    {
        typedef afl::container::DereferencingIterator<afl::container::MemberIterator<std::map<int,int*>::iterator,
                                                                                     int*,
                                                                                     &std::pair<const int,int*>::second> > dite;
        dite i = dite(imap.begin());
        a.checkEqual("mp 0", *i, 17); a.check("mp 0 end", i != dite(imap.end())); ++i;
        a.checkEqual("mp 1", *i, 42); a.check("mp 1 end", i != dite(imap.end())); ++i;
        a.checkEqual("mp 2", *i, 3);  a.check("mp 2 end", i != dite(imap.end())); ++i;
        a.check("mp 3 end", i == dite(imap.end()));
    }

    for (std::map<int,int*>::iterator p = imap.begin(); p != imap.end(); ++p) {
        delete p->second;
        p->second = 0;
    }
}
