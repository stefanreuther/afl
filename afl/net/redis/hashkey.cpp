/**
  *  \file afl/net/redis/hashkey.cpp
  *  \brief Class afl::net::redis::HashKey
  */

#include <memory>
#include "afl/net/redis/hashkey.hpp"
#include "afl/net/commandhandler.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/access.hpp"
#include "afl/net/redis/field.hpp"
#include "afl/net/redis/integerfield.hpp"
#include "afl/net/redis/stringfield.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::HashKey::HashKey(CommandHandler& ch, String_t name)
    : Key(ch, name)
{ }

// Get number of fields in hash (HLEN).
int32_t
afl::net::redis::HashKey::size() const
{
    return getHandler().callInt(Segment().pushBackString("HLEN").pushBackString(getName()));
}

// FIXME: port this?
// void
// DbHash::setFields(const StringList_t& vec)
// {
//     client.callVoid(DbRequest().withString("HMSET").withString(key).withInlineStringList(vec));
// }

// FIXME: port this?
// void
// DbHash::getAll(StringList_t& vec)
// {
//     std::auto_ptr<IntValue> val(client.call(DbRequest().withString("HGETALL").withString(key)));
//     toStringListResult(val.get(), vec);
// }

// FIXME: port this?
// void
// DbHash::getAll(IntList_t& vec)
// {
//     std::auto_ptr<IntValue> val(client.call(DbRequest().withString("HGETALL").withString(key)));
//     toIntListResult(val.get(), vec);
// }

// FIXME: port this?
// void
// DbHash::setAll(const StringList_t& vec)
// {
//     return client.callVoid(DbRequest().withString("HMSET").withString(key).withInlineStringList(vec));
// }

// Get names of all fields in hash (HKEYS).
void
afl::net::redis::HashKey::getFieldNames(afl::data::StringList_t& fieldNames) const
{
    std::auto_ptr<afl::data::Value> val(getHandler().call(Segment().pushBackString("HKEYS").pushBackString(getName())));
    afl::data::Access(val).toStringList(fieldNames);
}

// FIXME: port this?
// IntValue*
// DbHash::getFields(const StringList_t& vec)
// {
//     if (vec.empty()) {
//         Ptr<IntArrayData> ad = new IntArrayData();
//         ad->addDimension(0);
//         return new IntArray(ad);
//     } else {
//         return client.call(DbRequest().withString("HMGET").withString(key).withInlineStringList(vec));
//     }
// }

// Access a field.
afl::net::redis::Field
afl::net::redis::HashKey::field(const String_t& fieldName)
{
    return Field(*this, fieldName);
}

// Access integer field.
afl::net::redis::IntegerField
afl::net::redis::HashKey::intField(const String_t& fieldName)
{
    return IntegerField(*this, fieldName);
}

// Access string field.
afl::net::redis::StringField
afl::net::redis::HashKey::stringField(const String_t& fieldName)
{
    return StringField(*this, fieldName);
}
