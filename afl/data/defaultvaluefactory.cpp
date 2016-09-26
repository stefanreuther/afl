/**
  *  \file afl/data/defaultvaluefactory.cpp
  *  \brief Class afl::data::DefaultValueFactory
  */

#include "afl/data/defaultvaluefactory.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/data/errorvalue.hpp"

afl::data::DefaultValueFactory::DefaultValueFactory()
    : ValueFactory()
{ }

afl::data::DefaultValueFactory::~DefaultValueFactory()
{ }

afl::data::Value*
afl::data::DefaultValueFactory::createString(const String_t& sv)
{
    return new StringValue(sv);
}

afl::data::Value*
afl::data::DefaultValueFactory::createInteger(int32_t iv)
{
    return new IntegerValue(iv);
}

afl::data::Value*
afl::data::DefaultValueFactory::createFloat(double fv)
{
    return new FloatValue(fv);
}

afl::data::Value*
afl::data::DefaultValueFactory::createBoolean(bool bv)
{
    return new BooleanValue(bv);
}

afl::data::Value*
afl::data::DefaultValueFactory::createHash(NameMap& keys, Segment& values)
{
    // Create hash
    afl::base::Ptr<Hash> pHash(Hash::create(keys, values));

    // Create object
    return new HashValue(pHash);
}

afl::data::Value*
afl::data::DefaultValueFactory::createVector(Segment& values)
{
    // Create vector
    afl::base::Ptr<Vector> pVector(Vector::create(values));

    // Create object
    return new VectorValue(pVector);
}

afl::data::Value*
afl::data::DefaultValueFactory::createError(const String_t& source, const String_t& str)
{
    return new ErrorValue(source, str);
}

afl::data::Value*
afl::data::DefaultValueFactory::createNull()
{
    return 0;
}
