/**
  *  \file u/t_container_memberiterator.cpp
  *  \brief Test for afl::container::MemberIterator
  */

#include "afl/container/memberiterator.hpp"

#include <map>
#include "u/t_container.hpp"
#include "afl/container/dereferencingiterator.hpp"

/** Simple test. */
void
TestContainerMemberIterator::testIt()
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
        TS_ASSERT_EQUALS(*i, 1);  TS_ASSERT_DIFFERS(i, mite(imap.end())); ++i;
        TS_ASSERT_EQUALS(*i, 17); TS_ASSERT_DIFFERS(i, mite(imap.end())); ++i;
        TS_ASSERT_EQUALS(*i, 42); TS_ASSERT_DIFFERS(i, mite(imap.end())); ++i;
        TS_ASSERT_EQUALS(i, mite(imap.end()));
    }

    // Member + Pointer iterator
    {
        typedef afl::container::DereferencingIterator<afl::container::MemberIterator<std::map<int,int*>::iterator,
                                                                                     int*,
                                                                                     &std::pair<const int,int*>::second> > dite;
        dite i = dite(imap.begin());
        TS_ASSERT_EQUALS(*i, 17); TS_ASSERT_DIFFERS(i, dite(imap.end())); ++i;
        TS_ASSERT_EQUALS(*i, 42); TS_ASSERT_DIFFERS(i, dite(imap.end())); ++i;
        TS_ASSERT_EQUALS(*i, 3);  TS_ASSERT_DIFFERS(i, dite(imap.end())); ++i;
        TS_ASSERT_EQUALS(i, dite(imap.end()));
    }

    for (std::map<int,int*>::iterator p = imap.begin(); p != imap.end(); ++p) {
        delete p->second;
        p->second = 0;
    }
}
