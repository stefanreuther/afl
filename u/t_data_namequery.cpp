/**
  *  \file u/t_data_namequery.cpp
  *  \brief Test for afl::data::NameQuery
  */

#include "afl/data/namequery.hpp"

#include "u/t_data.hpp"

/** Simple tests. */
void
TestDataNameQuery::testIt()
{
    afl::data::NameQuery q("hello");

    // match
    TS_ASSERT(q.match("hello"));
    TS_ASSERT(q.match(String_t("hello")));

    // mismatch
    TS_ASSERT(!q.match(""));
    TS_ASSERT(!q.match(String_t()));

    // substring, match
    TS_ASSERT(afl::data::NameQuery(q, 2).match("llo"));

    // substring with out-of-range access
    TS_ASSERT(afl::data::NameQuery(q, 10).match(""));
    TS_ASSERT(!afl::data::NameQuery(q, 10).match("x"));

    // sorting
    TS_ASSERT(!q.before(""));
    TS_ASSERT(!q.before("hello"));
    TS_ASSERT(q.before("hello world"));
    TS_ASSERT(q.before("hf"));
}
