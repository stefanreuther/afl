/**
  *  \file test/afl/data/defaultvaluefactorytest.cpp
  *  \brief Test for afl::data::DefaultValueFactory
  */

#include "afl/data/defaultvaluefactory.hpp"

#include <memory>
#include "afl/data/booleanvalue.hpp"
#include "afl/data/errorvalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/namemap.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/value.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/test/testrunner.hpp"

namespace afl { namespace data {

    AFL_TEST("afl.data.DefaultValueFactory:createString", a)
    {
        DefaultValueFactory f;
        std::auto_ptr<Value> v(f.createString("abc"));

        a.check("result", v.get() != 0);
        a.check("type",   dynamic_cast<StringValue*>(v.get()) != 0);
        a.check("value",  dynamic_cast<StringValue*>(v.get())->getValue() == "abc");
    }

    AFL_TEST("afl.data.DefaultValueFactory:createInteger", a)
    {
        DefaultValueFactory f;
        std::auto_ptr<Value> v(f.createInteger(987));

        a.check("result", v.get() != 0);
        a.check("type",   dynamic_cast<IntegerValue*>(v.get()) != 0);
        a.check("value",  dynamic_cast<IntegerValue*>(v.get())->getValue() == 987);
    }

    AFL_TEST("afl.data.DefaultValueFactory:createFloat", a)
    {
        DefaultValueFactory f;
        std::auto_ptr<Value> v(f.createFloat(-2.5));

        a.check("result", v.get() != 0);
        a.check("type",   dynamic_cast<FloatValue*>(v.get()) != 0);
        a.check("value",  dynamic_cast<FloatValue*>(v.get())->getValue() == -2.5);
    }

    AFL_TEST("afl.data.DefaultValueFactory:createBoolean", a)
    {
        DefaultValueFactory f;
        std::auto_ptr<Value> v(f.createBoolean(true));

        a.check("result", v.get() != 0);
        a.check("type",   dynamic_cast<BooleanValue*>(v.get()) != 0);
        a.check("value",  dynamic_cast<BooleanValue*>(v.get())->getValue() == 1);
    }

    AFL_TEST("afl.data.DefaultValueFactory:createHash", a)
    {
        DefaultValueFactory f;

        // Keys
        NameMap keys;
        keys.add("a");
        keys.add("b");

        // Values
        Segment values;
        values.pushBackNew(f.createInteger(1));
        values.pushBackNew(f.createString("xyz"));

        // Make a hash
        std::auto_ptr<Value> v(f.createHash(keys, values));

        // Check it
        a.check("result", v.get() != 0);
        a.check("type", dynamic_cast<HashValue*>(v.get()) != 0);

        afl::base::Ref<Hash> hv(dynamic_cast<HashValue*>(v.get())->getValue());
        a.check("attr a", dynamic_cast<IntegerValue*>(hv->get("a")) != 0);
        a.check("attr b", dynamic_cast<StringValue*>(hv->get("b")) != 0);
    }

    AFL_TEST("afl.data.DefaultValueFactory:createVector", a)
    {
        DefaultValueFactory f;

        // Values
        Segment values;
        values.pushBackNew(f.createInteger(1));
        values.pushBackNew(f.createString("xyz"));

        // Make a hash
        std::auto_ptr<Value> v(f.createVector(values));

        // Check it
        a.check("result", v.get() != 0);
        a.check("type", dynamic_cast<VectorValue*>(v.get()) != 0);

        afl::base::Ref<Vector> vv(dynamic_cast<VectorValue*>(v.get())->getValue());
        a.check("index 0", dynamic_cast<IntegerValue*>(vv->get(0)) != 0);
        a.check("index 1", dynamic_cast<StringValue*>(vv->get(1)) != 0);
    }

    AFL_TEST("afl.data.DefaultValueFactory:createError", a)
    {
        DefaultValueFactory f;
        std::auto_ptr<Value> v(f.createError("source", "abc"));

        a.check("result", v.get() != 0);
        a.check("type",   dynamic_cast<ErrorValue*>(v.get()) != 0);
        a.check("text",   dynamic_cast<ErrorValue*>(v.get())->getText() == "abc");
        a.check("source", dynamic_cast<ErrorValue*>(v.get())->getSource() == "source");
    }

    AFL_TEST("afl.data.DefaultValueFactory:createNull", a)
    {
        DefaultValueFactory f;
        std::auto_ptr<Value> v(f.createNull());
        a.checkNull("", v.get());
    }

} }
