/**
  *  \file u/t_data_defaultvaluefactory.cpp
  *  \brief Test for afl::data::DefaultValueFactory
  */

#include "afl/data/defaultvaluefactory.hpp"

#include <memory>
#include "u/t_data.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/value.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/data/namemap.hpp"
#include "afl/data/errorvalue.hpp"

/** Test createString. */
void
TestDataDefaultValueFactory::testString()
{
    afl::data::DefaultValueFactory f;
    std::auto_ptr<afl::data::Value> v(f.createString("abc"));

    TS_ASSERT(v.get() != 0);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(v.get()) != 0);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(v.get())->getValue() == "abc");
}

/** Test createInteger. */
void
TestDataDefaultValueFactory::testInteger()
{
    afl::data::DefaultValueFactory f;
    std::auto_ptr<afl::data::Value> v(f.createInteger(987));

    TS_ASSERT(v.get() != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(v.get()) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(v.get())->getValue() == 987);
}

/** Test createFloat. */
void
TestDataDefaultValueFactory::testFloat()
{
    afl::data::DefaultValueFactory f;
    std::auto_ptr<afl::data::Value> v(f.createFloat(-2.5));

    TS_ASSERT(v.get() != 0);
    TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(v.get()) != 0);
    TS_ASSERT(dynamic_cast<afl::data::FloatValue*>(v.get())->getValue() == -2.5);
}

/** Test createBoolean. */
void
TestDataDefaultValueFactory::testBoolean()
{
    afl::data::DefaultValueFactory f;
    std::auto_ptr<afl::data::Value> v(f.createBoolean(true));

    TS_ASSERT(v.get() != 0);
    TS_ASSERT(dynamic_cast<afl::data::BooleanValue*>(v.get()) != 0);
    TS_ASSERT(dynamic_cast<afl::data::BooleanValue*>(v.get())->getValue() == 1);
}

/** Test createHash. */
void
TestDataDefaultValueFactory::testHash()
{
    afl::data::DefaultValueFactory f;

    // Keys
    afl::data::NameMap keys;
    keys.add("a");
    keys.add("b");

    // Values
    afl::data::Segment values;
    values.pushBackNew(f.createInteger(1));
    values.pushBackNew(f.createString("xyz"));

    // Make a hash
    std::auto_ptr<afl::data::Value> v(f.createHash(keys, values));

    // Check it
    TS_ASSERT(v.get() != 0);
    TS_ASSERT(dynamic_cast<afl::data::HashValue*>(v.get()) != 0);

    afl::base::Ptr<afl::data::Hash> hv(dynamic_cast<afl::data::HashValue*>(v.get())->getValue());
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(hv->get("a")) != 0)
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(hv->get("b")) != 0)
}

/** Test createVector. */
void
TestDataDefaultValueFactory::testVector()
{
    afl::data::DefaultValueFactory f;

    // Values
    afl::data::Segment values;
    values.pushBackNew(f.createInteger(1));
    values.pushBackNew(f.createString("xyz"));

    // Make a hash
    std::auto_ptr<afl::data::Value> v(f.createVector(values));

    // Check it
    TS_ASSERT(v.get() != 0);
    TS_ASSERT(dynamic_cast<afl::data::VectorValue*>(v.get()) != 0);

    afl::base::Ptr<afl::data::Vector> vv(dynamic_cast<afl::data::VectorValue*>(v.get())->getValue());
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(vv->get(0)) != 0)
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(vv->get(1)) != 0)
}

/** Test createError. */
void
TestDataDefaultValueFactory::testError()
{
    afl::data::DefaultValueFactory f;
    std::auto_ptr<afl::data::Value> v(f.createError("source", "abc"));

    TS_ASSERT(v.get() != 0);
    TS_ASSERT(dynamic_cast<afl::data::ErrorValue*>(v.get()) != 0);
    TS_ASSERT(dynamic_cast<afl::data::ErrorValue*>(v.get())->getText() == "abc");
    TS_ASSERT(dynamic_cast<afl::data::ErrorValue*>(v.get())->getSource() == "source");
}

/** Test createNull. */
void
TestDataDefaultValueFactory::testNull()
{
    afl::data::DefaultValueFactory f;
    std::auto_ptr<afl::data::Value> v(f.createNull());
    TS_ASSERT(v.get() == 0);
}
