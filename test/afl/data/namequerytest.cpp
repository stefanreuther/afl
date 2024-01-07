/**
  *  \file test/afl/data/namequerytest.cpp
  *  \brief Test for afl::data::NameQuery
  */

#include "afl/data/namequery.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.NameQuery", a)
{
    afl::data::NameQuery q("hello");

    // match
    a.check("01", q.match("hello"));
    a.check("02", q.match(String_t("hello")));

    // mismatch
    a.check("11", !q.match(""));
    a.check("12", !q.match(String_t()));

    // substring, match
    a.check("21", afl::data::NameQuery(q, 2).match("llo"));

    // substring with out-of-range access
    a.check("31", afl::data::NameQuery(q, 10).match(""));
    a.check("32", !afl::data::NameQuery(q, 10).match("x"));

    // sorting
    a.check("41", !q.before(""));
    a.check("42", !q.before("hello"));
    a.check("43", q.before("hello world"));
    a.check("44", q.before("hf"));
}
