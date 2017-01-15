/**
  *  \file afl/net/redis/internaldatabase.cpp
  *  \brief Class afl::net::redis::InternalDatabase
  *
  *  \todo This is a quick implementation. Can we factor the large if/else chain into something nicer?
  *
  *  \todo This uses a custom data structure.
  *  It would make much sense to implement this using afl::data structures as a backend.
  *  Main question to solve is how do we represent ZSets?
  */

#include <memory>
#include <list>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/data/segmentview.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/string/parse.hpp"
#include "afl/string/format.hpp"
#include "afl/data/stringlist.hpp"
#include "afl/sys/mutexguard.hpp"

/*
 *  Imports
 */

using afl::except::RemoteErrorException;

/*
 *  Error messages
 */

const char MISSING_COMMAND[] = "Missing command";
const char INVALID_PARAMETER_COUNT[] = "Invalid number of parameters";
const char INVALID_OPTION[] = "Invalid option";
const char INVALID_TYPE[] = "Invalid type";
const char INVALID_TYPE_INT[] = "Invalid type (expect integer)";
const char INVALID_INDEX[] = "Invalid index";
const char INVALID_COMMAND[] = "Invalid command";
const char KEY_NOT_FOUND[] = "Key not found";

const char INTERNAL_DATABASE[] = "<InternalDatabase>";


/*
 *  Local classes
 */

// Base class for values
class afl::net::redis::InternalDatabase::Key : public afl::base::Deletable {
 public:
    // Get type as a statically allocated string
    virtual const char* getType() = 0;
};

// String value
class afl::net::redis::InternalDatabase::String : public Key {
 public:
    String()
        : m_string()
        { }
    String(const String_t& str)
        : m_string(str)
        { }
    const char* getType()
        { return "string"; }

    // Modify numeric value (implements INCR, DECR, INCRBY, DECRBY).
    int32_t modify(int32_t delta)
        {
            int32_t intVal = 0;
            if (!m_string.empty() && !afl::string::strToInteger(m_string, intVal)) {
                throw RemoteErrorException(INTERNAL_DATABASE, INVALID_TYPE_INT);
            }
            intVal += delta;
            m_string = afl::string::Format("%d", intVal);
            return intVal;
        }

    String_t m_string;
};

// Hash value
class afl::net::redis::InternalDatabase::Hash : public Key {
 public:
    std::map<String_t, String_t> m_hash;
    const char* getType()
        { return "hash"; }
};

// Base class for sortable values
class afl::net::redis::InternalDatabase::Sortable : public Key {
 public:
    // Get origin values for sorting this key
    virtual void getValues(afl::data::StringList_t& out) = 0;
};

// List value
class afl::net::redis::InternalDatabase::List : public Sortable {
 public:
    std::list<String_t> m_list;
    const char* getType()
        { return "list"; }

    // Convert user-specified index into actual position.
    size_t convertIndex(int32_t n)
        {
            if (n >= 0) {
                return size_t(n);
            } else {
                return size_t(m_list.size() + n);
            }
        }

    // Get value by index. Returns true on success.
    bool getByIndex(size_t index, String_t& out) const
        {
            std::list<String_t>::const_iterator i = m_list.begin();
            while (index > 0 && i != m_list.end()) {
                ++i, --index;
            }
            if (i != m_list.end()) {
                out = *i;
                return true;
            } else {
                return false;
            }
        }

    // Set value by index. Returns true on success.
    bool setByIndex(size_t index, const String_t& val)
        {
            std::list<String_t>::iterator i = m_list.begin();
            while (index > 0 && i != m_list.end()) {
                ++i, --index;
            }
            if (i != m_list.end()) {
                *i = val;
                return true;
            } else {
                return false;
            }
        }

    // Sortable method.
    void getValues(afl::data::StringList_t& out)
        {
            for (std::list<String_t>::iterator it = m_list.begin(), e = m_list.end(); it != e; ++it) {
                out.push_back(*it);
            }
        }
};

// Set value
class afl::net::redis::InternalDatabase::Set : public Sortable {
 public:
    std::set<String_t> m_set;
    const char* getType()
        { return "set"; }

    // Fetch a random element. Delete it if remove is true. Returns true on success.
    bool getRandomElement(String_t& out, bool remove)
        {
            if (m_set.empty()) {
                return false;
            } else {
                std::set<String_t>::iterator it = m_set.begin();
                std::advance(it, std::rand() % m_set.size());
                out = *it;
                if (remove) {
                    m_set.erase(it);
                }
                return true;
            }
        }

    // Sortable method-
    void getValues(afl::data::StringList_t& out)
        {
            for (std::set<String_t>::iterator it = m_set.begin(), e = m_set.end(); it != e; ++it) {
                out.push_back(*it);
            }
        }
};

// Sort predicate.
class afl::net::redis::InternalDatabase::Sorter {
 public:
    Sorter(InternalDatabase& db, const String_t& pattern, bool descending, bool alpha)
        : m_db(db),
          m_pattern(pattern),
          m_descending(descending),
          m_alpha(alpha)
        { }

    bool operator()(const String_t& a, const String_t& b)
        {
            // Fetch values. If one value fails, this keeps the string empty.
            String_t as, bs;
            m_db.getSortValue(a, m_pattern, m_descending ? bs : as);
            m_db.getSortValue(b, m_pattern, m_descending ? as : bs);

            // Compare
            if (m_alpha) {
                return as < bs;
            } else {
                int32_t ai = 0, bi = 0;
                if (!(as == "" || afl::string::strToInteger(as, ai)) || !(bs == "" || afl::string::strToInteger(bs, bi))) {
                    throw RemoteErrorException(INTERNAL_DATABASE, INVALID_TYPE_INT);
                }
                return ai < bi;
            }
        }

 private:
    InternalDatabase& m_db;
    const String_t m_pattern;
    const bool m_descending;
    const bool m_alpha;
};


namespace {
    void fail(const char* msg)
    {
        throw RemoteErrorException(INTERNAL_DATABASE, msg);
    }

    void checkArgumentCount(const afl::data::SegmentView& v, size_t need)
    {
        if (v.size() != need) {
            fail(INVALID_PARAMETER_COUNT);
        }
    }

    void checkArgumentCountAtLeast(const afl::data::SegmentView& v, size_t need)
    {
        if (v.size() < need) {
            fail(INVALID_PARAMETER_COUNT);
        }
    }

    bool matchKey(const String_t& pat, const String_t& key)
    {
        String_t::size_type n = pat.find('*');
        if (n == String_t::npos) {
            return pat == key;
        } else {
            String_t::size_type endLength = pat.size() - n - 1;
            return key.size() >= pat.size() - 1
                && pat.compare(0, n, key, 0, n) == 0
                && pat.compare(pat.size() - endLength, endLength, key, key.size() - endLength, endLength) == 0;
        }
    }
}

/**************************** InternalDatabase ***************************/

// Constructor.
afl::net::redis::InternalDatabase::InternalDatabase()
    : m_mutex(),
      m_data()
{ }

// Destructor.
afl::net::redis::InternalDatabase::~InternalDatabase()
{ }

// CommandHandler: call, with value return.
afl::net::redis::InternalDatabase::Value_t*
afl::net::redis::InternalDatabase::call(const Segment_t& command)
{
    // Protect it
    afl::sys::MutexGuard g(m_mutex);

    // Value factory, for reference
    afl::data::DefaultValueFactory factory;

    // Read the command
    afl::data::SegmentView v(command);
    String_t verb;
    if (!v.eat(verb)) {
        fail(MISSING_COMMAND);
    }
    verb = afl::string::strUCase(verb);

    // Placeholder for generic args
    String_t keyArg;
    String_t fieldArg;
    String_t stringArg;
    int32_t intArg = 0;

    // Dispatch commands
    if (verb == "APPEND") {
        // APPEND key value
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        String& sk = getCreate<String>(keyArg);
        sk.m_string += stringArg;
        return factory.createInteger(int32_t(sk.m_string.size()));
    } else if (verb == "DECR") {
        // DECR key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        return factory.createInteger(getCreate<String>(keyArg).modify(-1));
    } else if (verb == "DECRBY") {
        // DECRBY key delta
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(intArg);

        return factory.createInteger(getCreate<String>(keyArg).modify(-intArg));
    } else if (verb == "DEL") {
        // DEL key...
        int32_t n = 0;
        while (v.size() > 0) {
            v.eat(keyArg);
            if (m_data.erase(keyArg)) {
                ++n;
            }
        }
        return factory.createInteger(n);
    } else if (verb == "EXISTS") {
        // EXISTS key...
        int32_t n = 0;
        while (v.size() > 0) {
            v.eat(keyArg);
            if (m_data.find(keyArg) != m_data.end()) {
                ++n;
            }
        }
        return factory.createInteger(n);
    } else if (verb == "GET") {
        // GET key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        if (String* sk = get<String>(keyArg)) {
            return factory.createString(sk->m_string);
        } else {
            return factory.createNull();
        }
    } else if (verb == "GETRANGE") {
        // GETRANGE key first last
        int32_t beg = 0, end = 0;
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(beg);
        v.eat(end);

        if (String* sk = get<String>(keyArg)) {
            // This implementation of the range limitation means that
            // "GETRANGE key -$bignum -$bignum" will truncate both locations to 0 and thus return the first character of the value.
            if (beg < 0) {
                beg += int32_t(sk->m_string.size());
            }
            if (end < 0) {
                end += int32_t(sk->m_string.size());
            }
            if (beg < 0) {
                beg = 0;
            }
            if (end < 0) {
                end = 0;
            }
            ++end;
            if (beg >= end || beg >= int32_t(sk->m_string.size())) {
                return factory.createString(String_t());
            } else {
                return factory.createString(sk->m_string.substr(beg, end-beg));
            }
        } else {
            return factory.createNull();
        }
    } else if (verb == "GETSET") {
        // GETSET key value
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        // "get" part
        std::auto_ptr<Value_t> result;
        if (String* sk = get<String>(keyArg)) {
            result.reset(factory.createString(sk->m_string));
        } else {
            result.reset(factory.createNull());
        }

        // "set" part
        getCreate<String>(keyArg).m_string = stringArg;
        return result.release();
    } else if (verb == "HDEL") {
        // HDEL key field...
        checkArgumentCountAtLeast(v, 1);
        v.eat(keyArg);
        int32_t n = 0;

        if (Hash* hk = get<Hash>(keyArg)) {
            while (v.size() > 0) {
                v.eat(fieldArg);
                if (hk->m_hash.erase(fieldArg)) {
                    ++n;
                }
            }
            if (hk->m_hash.empty()) {
                m_data.erase(keyArg);
            }
        }
        return factory.createInteger(n);
    } else if (verb == "HEXISTS") {
        // HEXISTS key field...
        checkArgumentCountAtLeast(v, 1);
        v.eat(keyArg);
        int32_t n = 0;

        if (Hash* hk = get<Hash>(keyArg)) {
            while (v.size() > 0) {
                v.eat(keyArg);
                if (hk->m_hash.find(keyArg) != hk->m_hash.end()) {
                    ++n;
                }
            }
        }
        return factory.createInteger(n);
    } else if (verb == "HGET") {
        // HGET key field
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(fieldArg);

        if (Hash* hk = get<Hash>(keyArg)) {
            std::map<String_t, String_t>::iterator it = hk->m_hash.find(fieldArg);
            if (it != hk->m_hash.end()) {
                return factory.createString(it->second);
            } else {
                return factory.createNull();
            }
        } else {
            return factory.createNull();
        }
    } else if (verb == "HINCRBY") {
        // HINCRBY key field value
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(fieldArg);
        v.eat(intArg);

        Hash& hk = getCreate<Hash>(keyArg);
        String_t stringVal = hk.m_hash[fieldArg];
        int32_t intVal = 0;
        if (!stringVal.empty() && !afl::string::strToInteger(stringVal, intVal)) {
            fail(INVALID_TYPE_INT);
        }
        intVal += intArg;
        hk.m_hash[fieldArg] = afl::string::Format("%d", intVal);
        return factory.createInteger(intVal);
    } else if (verb == "HKEYS") {
        // HKEYS key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        Segment_t result;
        if (Hash* hk = get<Hash>(keyArg)) {
            for (std::map<String_t, String_t>::const_iterator it = hk->m_hash.begin(), e = hk->m_hash.end(); it != e; ++it) {
                result.pushBackString(it->first);
            }
        }
        return factory.createVector(result);
    } else if (verb == "HLEN") {
        // HLEN key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        int32_t result = 0;
        if (Hash* hk = get<Hash>(keyArg)) {
            result = int32_t(hk->m_hash.size());
        }
        return factory.createInteger(result);
    } else if (verb == "HMSET") {
        // HMSET key field value...
        if ((v.size() % 2) != 1) {
            fail(INVALID_PARAMETER_COUNT);
        }
        v.eat(keyArg);
        Hash& hk = getCreate<Hash>(keyArg);

        while (v.size() > 0) {
            v.eat(fieldArg);
            v.eat(stringArg);
            hk.m_hash[fieldArg] = stringArg;
        }
        return factory.createString("OK");
    } else if (verb == "HSET") {
        // HSET key field value
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(fieldArg);
        v.eat(stringArg);

        Hash& hk = getCreate<Hash>(keyArg);
        int32_t result = (hk.m_hash.find(fieldArg) == hk.m_hash.end());
        hk.m_hash[fieldArg] = stringArg;
        return factory.createInteger(result);
    } else if (verb == "INCR") {
        // INCR key
        checkArgumentCount(v, 1);
        v.eat(keyArg);
        return factory.createInteger(getCreate<String>(keyArg).modify(+1));
    } else if (verb == "INCRBY") {
        // INCRBY key delta
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(intArg);
        return factory.createInteger(getCreate<String>(keyArg).modify(intArg));
    } else if (verb == "KEYS") {
        // KEYS pat*
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        Segment_t result;
        for (Data_t::iterator it = m_data.begin(), e = m_data.end(); it != e; ++it) {
            if (matchKey(keyArg, it->first)) {
                result.pushBackString(it->first);
            }
        }
        return factory.createVector(result);
    } else if (verb == "LINDEX") {
        // LINDEX key index
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(intArg);

        if (List* lk = get<List>(keyArg)) {
            size_t n = lk->convertIndex(intArg);
            String_t result;
            if (lk->getByIndex(n, result)) {
                return factory.createString(result);
            } else {
                return factory.createNull();
            }
        } else {
            return factory.createNull();
        }
    } else if (verb == "LLEN") {
        // LLEN key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        int32_t result = 0;
        if (List* lk = get<List>(keyArg)) {
            result = int32_t(lk->m_list.size());
        }
        return factory.createInteger(result);
    } else if (verb == "LPOP") {
        // LPOP key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        if (List* lk = get<List>(keyArg)) {
            if (lk->m_list.empty()) {
                // Cannot happen
                return factory.createNull();
            } else {
                // OK, we have an element
                String_t result = lk->m_list.front();
                lk->m_list.pop_front();
                if (lk->m_list.empty()) {
                    m_data.erase(keyArg);
                }
                return factory.createString(result);
            }
        } else {
            return factory.createNull();
        }
    } else if (verb == "LPUSH") {
        // LPUSH key args...
        checkArgumentCountAtLeast(v, 2);
        v.eat(keyArg);

        List& lk = getCreate<List>(keyArg);
        while (v.size() > 0) {
            v.eat(stringArg);
            lk.m_list.push_front(stringArg);
        }
        return factory.createInteger(int32_t(lk.m_list.size()));
    } else if (verb == "LRANGE") {
        // LRANGE key beg end
        int32_t beg = 0, end = 0;
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(beg);
        v.eat(end);

        Segment_t result;
        if (List* lk = get<List>(keyArg)) {
            size_t i = lk->convertIndex(beg);
            size_t j = lk->convertIndex(end);
            while (i <= j && lk->getByIndex(i, stringArg)) {
                result.pushBackString(stringArg);
                ++i;
            }
        }
        return factory.createVector(result);
    } else if (verb == "LREM") {
        // LREM key count value
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(intArg);
        v.eat(stringArg);

        int32_t result = 0;
        if (List* lk = get<List>(keyArg)) {
            if (intArg < 0) {
                for (std::list<String_t>::iterator i = lk->m_list.end(); i != lk->m_list.begin(); /* done in loop body */) {
                    --i;
                    if (*i == stringArg) {
                        i = lk->m_list.erase(i);
                        ++result;
                        if (++intArg == 0) {
                            break;
                        }
                    }
                }
            } else {
                for (std::list<String_t>::iterator i = lk->m_list.begin(); i != lk->m_list.end(); /* done in loop body */) {
                    if (*i == stringArg) {
                        i = lk->m_list.erase(i);
                        ++result;
                        if (--intArg == 0) {
                            // This automatically never breaks if intArg == 0
                            break;
                        }
                    } else {
                        ++i;
                    }
                }
            }
        }
        return factory.createInteger(result);
    } else if (verb == "LSET") {
        // LSET key index value
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(intArg);
        v.eat(stringArg);

        List* lk = get<List>(keyArg);
        if (lk == 0 || !lk->setByIndex(lk->convertIndex(intArg), stringArg)) {
            fail(INVALID_INDEX);
        }
        return factory.createString("OK");
    } else if (verb == "LTRIM") {
        // LTRIM key beg end
        int32_t beg = 0, end = 0;
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(beg);
        v.eat(end);

        if (List* lk = get<List>(keyArg)) {
            size_t i = lk->convertIndex(beg);
            size_t j = lk->convertIndex(end);
            while (!lk->m_list.empty() && i > 0) {
                lk->m_list.pop_front();
                --i;
                if (j > 0) {
                    --j;
                }
            }
            while (lk->m_list.size() > j+1) {
                lk->m_list.pop_back();
            }
            if (lk->m_list.empty()) {
                m_data.erase(keyArg);
            }
        }
        return factory.createString("OK");
    } else if (verb == "MSET") {
        // MSET key value...
        if ((v.size() % 2) != 0) {
            fail(INVALID_PARAMETER_COUNT);
        }
        while (v.size() > 0) {
            v.eat(keyArg);
            v.eat(stringArg);
            m_data.erase(keyArg);
            getCreate<String>(keyArg).m_string = stringArg;
        }
        return factory.createString("OK");
    } else if (verb == "RENAME") {
        // RENAME old new
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        Data_t::iterator keyIt = m_data.find(keyArg);
        if (keyIt == m_data.end()) {
            fail(KEY_NOT_FOUND);
        }
        m_data.insertNew(stringArg, m_data.extractElement(keyIt));
        return factory.createString("OK");
    } else if (verb == "RENAMENX") {
        // RENAMENX old new
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        Data_t::iterator keyIt = m_data.find(keyArg);
        if (keyIt == m_data.end()) {
            fail(KEY_NOT_FOUND);
        }

        int32_t result = 0;
        if (m_data.find(stringArg) == m_data.end()) {
            m_data.insertNew(stringArg, m_data.extractElement(keyIt));
            result = 1;
        }
        return factory.createInteger(result);
    } else if (verb == "RPOP") {
        // RPOP key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        if (List* lk = get<List>(keyArg)) {
            if (lk->m_list.empty()) {
                // Cannot happen
                return factory.createNull();
            } else {
                // OK, we have an element
                String_t result = lk->m_list.back();
                lk->m_list.pop_back();
                if (lk->m_list.empty()) {
                    m_data.erase(keyArg);
                }
                return factory.createString(result);
            }
        } else {
            return factory.createNull();
        }
    } else if (verb == "RPOPLPUSH") {
        // RPOPLPUSH key otherKey
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        if (List* lk = get<List>(keyArg)) {
            if (lk->m_list.empty()) {
                // Cannot happen
                return factory.createNull();
            } else {
                // OK, we have an element
                String_t result = lk->m_list.back();
                getCreate<List>(stringArg).m_list.push_front(result);
                lk->m_list.pop_back();
                if (lk->m_list.empty()) {
                    m_data.erase(keyArg);
                }
                return factory.createString(result);
            }
        } else {
            return factory.createNull();
        }
    } else if (verb == "RPUSH") {
        // RPUSH key value...
        checkArgumentCountAtLeast(v, 2);
        v.eat(keyArg);

        List& lk = getCreate<List>(keyArg);
        while (v.size() > 0) {
            v.eat(stringArg);
            lk.m_list.push_back(stringArg);
        }
        return factory.createInteger(int32_t(lk.m_list.size()));
    } else if (verb == "SADD") {
        // SADD key value...
        checkArgumentCountAtLeast(v, 2);
        v.eat(keyArg);

        int32_t result = 0;
        Set& sk = getCreate<Set>(keyArg);
        while (v.size() > 0) {
            v.eat(stringArg);
            if (sk.m_set.find(stringArg) == sk.m_set.end()) {
                ++result;
                sk.m_set.insert(stringArg);
            }
        }
        return factory.createInteger(result);
    } else if (verb == "SCARD") {
        // SCARD set
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        int32_t result = 0;
        if (Set* sk = get<Set>(keyArg)) {
            result = int32_t(sk->m_set.size());
        }
        return factory.createInteger(result);
    } else if (verb == "SDIFF") {
        // SDIFF set...
        checkArgumentCountAtLeast(v, 1);

        std::set<String_t> set;
        executeSetOperation(Difference, set, v);

        Segment_t result;
        for (std::set<String_t>::iterator it = set.begin(), e = set.end(); it != e; ++it) {
            result.pushBackString(*it);
        }
        return factory.createVector(result);
    } else if (verb == "SDIFFSTORE") {
        // SDIFFSTORE dest set...
        checkArgumentCountAtLeast(v, 2);
        v.eat(keyArg);

        std::set<String_t> set;
        executeSetOperation(Difference, set, v);
        getCreate<Set>(keyArg).m_set = set;
        if (set.empty()) {
            m_data.erase(keyArg);
        }
        return factory.createInteger(int32_t(set.size()));
    } else if (verb == "SET") {
        // SET key value
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        m_data.erase(keyArg);
        getCreate<String>(keyArg).m_string = stringArg;
        return factory.createString("OK");
    } else if (verb == "SETNX") {
        // SETNX key value
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        int32_t result;
        if (get<String>(keyArg) != 0) {
            result = 0;
        } else {
            m_data.insertNew(keyArg, new String(stringArg));
            result = 1;
        }
        return factory.createInteger(result);
    } else if (verb == "SINTER") {
        // SINTER set...
        checkArgumentCountAtLeast(v, 1);

        std::set<String_t> set;
        executeSetOperation(Intersection, set, v);

        Segment_t result;
        for (std::set<String_t>::iterator it = set.begin(), e = set.end(); it != e; ++it) {
            result.pushBackString(*it);
        }
        return factory.createVector(result);
    } else if (verb == "SINTERSTORE") {
        // SINTERSTORE dest set...
        checkArgumentCountAtLeast(v, 2);
        v.eat(keyArg);

        std::set<String_t> set;
        executeSetOperation(Intersection, set, v);
        getCreate<Set>(keyArg).m_set = set;
        if (set.empty()) {
            m_data.erase(keyArg);
        }
        return factory.createInteger(int32_t(set.size()));
    } else if (verb == "SISMEMBER") {
        // SISMEMBER key value
        checkArgumentCount(v, 2);
        v.eat(keyArg);
        v.eat(stringArg);

        int32_t result = 0;
        if (Set* sk = get<Set>(keyArg)) {
            result = (sk->m_set.find(stringArg) != sk->m_set.end());
        }
        return factory.createInteger(result);
    } else if (verb == "SMEMBERS") {
        // SMEMBERS set
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        Segment_t result;
        if (Set* sk = get<Set>(keyArg)) {
            for (std::set<String_t>::iterator it = sk->m_set.begin(), e = sk->m_set.end(); it != e; ++it) {
                result.pushBackString(*it);
            }
        }
        return factory.createVector(result);
    } else if (verb == "SMOVE") {
        // SMOVE key dest value
        String_t dest;
        checkArgumentCount(v, 3);
        v.eat(keyArg);
        v.eat(dest);
        v.eat(stringArg);

        int32_t result = 0;
        if (Set* sk = get<Set>(keyArg)) {
            if (sk->m_set.erase(stringArg)) {
                result = 1;
                getCreate<Set>(dest).m_set.insert(stringArg);
                if (sk->m_set.empty()) {
                    m_data.erase(keyArg);
                }
            }
        }
        return factory.createInteger(result);
    } else if (verb == "SORT") {
        // SORT key [BY pattern] [LIMIT offset count] [GET pattern [GET pattern ...]] [ASC|DESC] [ALPHA] [STORE destination]
        checkArgumentCountAtLeast(v, 1);
        v.eat(keyArg);

        Segment_t result;
        executeSortOperation(get<Sortable>(keyArg), result, v);
        return factory.createVector(result);
    } else if (verb == "SPOP" || verb == "SRANDMEMBER") {
        // SPOP key
        // SRANDMEMBER key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        String_t result;
        Set* sk = get<Set>(keyArg);
        if (sk != 0 && sk->getRandomElement(result, (verb=="SPOP"))) {
            if (sk->m_set.empty()) {
                m_data.erase(keyArg);
            }
            return factory.createString(result);
        } else {
            return factory.createNull();
        }
    } else if (verb == "SREM") {
        // SREM key value...
        checkArgumentCountAtLeast(v, 2);
        v.eat(keyArg);

        int32_t result = 0;
        if (Set* sk = get<Set>(keyArg)) {
            while (v.size() > 0) {
                v.eat(stringArg);
                if (sk->m_set.erase(stringArg)) {
                    ++result;
                }
            }
            if (sk->m_set.empty()) {
                m_data.erase(keyArg);
            }
        }
        return factory.createInteger(result);
    } else if (verb == "STRLEN") {
        // STRLEN key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        if (String* sk = get<String>(keyArg)) {
            return factory.createInteger(int32_t(sk->m_string.size()));
        } else {
            return factory.createInteger(0);
        }
    } else if (verb == "SUNION") {
        // SUNION set...
        checkArgumentCountAtLeast(v, 1);

        std::set<String_t> set;
        executeSetOperation(Union, set, v);

        Segment_t result;
        for (std::set<String_t>::iterator it = set.begin(), e = set.end(); it != e; ++it) {
            result.pushBackString(*it);
        }
        return factory.createVector(result);
    } else if (verb == "SUNIONSTORE") {
        // SUNIONSTORE dest set...
        checkArgumentCountAtLeast(v, 2);
        v.eat(keyArg);

        std::set<String_t> set;
        executeSetOperation(Union, set, v);
        getCreate<Set>(keyArg).m_set = set;
        if (set.empty()) {
            m_data.erase(keyArg);
        }
        return factory.createInteger(int32_t(set.size()));
    } else if (verb == "TYPE") {
        // TYPE key
        checkArgumentCount(v, 1);
        v.eat(keyArg);

        Key* k = get<Key>(keyArg);
        if (k == 0) {
            return factory.createString("none");
        } else {
            return factory.createString(k->getType());
        }
    } else {
        fail(INVALID_COMMAND);
        return 0;
    }
}

// CommandHandler: call, without value return.
void
afl::net::redis::InternalDatabase::callVoid(const Segment_t& command)
{
    // Minimum implementation
    delete call(command);
}

template<typename T>
T*
afl::net::redis::InternalDatabase::get(const String_t& name) const
{
    Key* k = m_data[name];
    if (k != 0) {
        T* tk = dynamic_cast<T*>(k);
        if (!tk) {
            fail(INVALID_TYPE);
        }
        return tk;
    } else {
        return 0;
    }
}

template<typename T>
T&
afl::net::redis::InternalDatabase::getCreate(const String_t& name)
{
    T* tk = get<T>(name);
    if (tk == 0) {
        tk = new T();
        m_data.insertNew(name, tk);
    }
    return *tk;
}

void
afl::net::redis::InternalDatabase::executeSetOperation(SetOperation op, std::set<String_t>& out, afl::data::SegmentView& v)
{
    String_t key;
    assert(v.size() > 0);
    v.eat(key);
    if (Set* sk = get<Set>(key)) {
        out = sk->m_set;
    }

    while (v.size() > 0) {
        // Fetch one key
        v.eat(key);
        std::set<String_t> set;
        if (Set* sk = get<Set>(key)) {
            set = sk->m_set;
        }

        // Process it
        switch (op) {
         case Difference:
            for (std::set<String_t>::iterator it = out.begin(), e = out.end(); it != e; /* increment in loop */) {
                if (set.find(*it) != set.end()) {
                    out.erase(it++);
                } else {
                    ++it;
                }
            }
            break;

         case Intersection:
            for (std::set<String_t>::iterator it = out.begin(), e = out.end(); it != e; /* increment in loop */) {
                if (set.find(*it) == set.end()) {
                    out.erase(it++);
                } else {
                    ++it;
                }
            }
            break;

         case Union:
            for (std::set<String_t>::iterator it = set.begin(), e = set.end(); it != e; ++it) {
                out.insert(*it);
            }
            break;
        }
    }
}

void
afl::net::redis::InternalDatabase::executeSortOperation(Sortable* key, Segment_t& out, afl::data::SegmentView& v)
{
    // Parameters
    String_t pattern = "#";
    std::vector<String_t> get;

    bool limit = false;
    int32_t offset = 0;
    int32_t count = 0;

    bool descending = false;
    bool alpha = false;

    bool store = false;
    String_t storeKey;

    // Parse parameters
    while (v.size() > 0) {
        String_t flag;
        v.eat(flag);
        flag = afl::string::strUCase(flag);
        if (flag == "BY") {
            checkArgumentCountAtLeast(v, 1);
            v.eat(pattern);
        } else if (flag == "LIMIT") {
            checkArgumentCountAtLeast(v, 2);
            v.eat(offset);
            v.eat(count);
            limit = true;
        } else if (flag == "GET") {
            checkArgumentCountAtLeast(v, 1);
            String_t n;
            v.eat(n);
            get.push_back(n);
        } else if (flag == "ASC") {
            descending = false;
        } else if (flag == "DESC") {
            descending = true;
        } else if (flag == "ALPHA") {
            alpha = true;
        } else if (flag == "STORE") {
            checkArgumentCountAtLeast(v, 1);
            v.eat(storeKey);
            store = true;
        } else {
            fail(INVALID_OPTION);
        }
    }
    if (get.empty()) {
        get.push_back("#");
    }

    // Perform the sort operation
    afl::data::StringList_t originValues;
    if (key) {
        key->getValues(originValues);
    }
    std::sort(originValues.begin(), originValues.end(), Sorter(*this, pattern, descending, alpha));

    // Produce output
    if (!limit) {
        offset = 0;
        count = int32_t(originValues.size());
    }
    if (store) {
        // Store into a list key. This will overwrite anything, so clean the slot first.
        m_data.erase(storeKey);

        // If we have origin values, produce output. This will never produce an empty list, as we always have get's.
        if (!originValues.empty()) {
            List& lk = getCreate<List>(storeKey);
            for (int32_t i = 0; i < count && offset < int32_t(originValues.size()); ++i, ++offset) {
                for (std::vector<String_t>::const_iterator it = get.begin(), e = get.end(); it != e; ++it) {
                    String_t tmp;
                    getSortValue(originValues[i], *it, tmp);
                    lk.m_list.push_back(tmp);
                }
            }
        }
        // FIXME: this differs from redis: redis will return a number of elements, whereas this returns an empty list.
    } else {
        // Store to set for user
        for (int32_t i = 0; i < count && offset < int32_t(originValues.size()); ++i, ++offset) {
            for (std::vector<String_t>::const_iterator it = get.begin(), e = get.end(); it != e; ++it) {
                String_t tmp;
                if (getSortValue(originValues[i], *it, tmp)) {
                    out.pushBackString(tmp);
                } else {
                    out.pushBackNew(0);
                }
            }
        }
    }
}

bool
afl::net::redis::InternalDatabase::getSortValue(const String_t& originValue, String_t pattern, String_t& result)
{
    if (pattern == "#") {
        result = originValue;
        return true;
    } else {
        // Replace the "*".
        // Note that this implementation allows more than actual redis would allow:
        // - we accept "*" on either side of an arrow, i.e. "user:*:profile->name" or "usernames->*".
        //   redis requires the "*" in the key name (left side).
        // - we accept zero or one "*". redis requires exactly one.
        String_t::size_type n = pattern.find('*');
        if (n != String_t::npos) {
            pattern.replace(n, 1, originValue);
        }
        n = pattern.find("->");
        if (n != String_t::npos) {
            if (Hash* hk = dynamic_cast<Hash*>(m_data[pattern.substr(0, n)])) {
                std::map<String_t, String_t>::const_iterator it = hk->m_hash.find(pattern.substr(n+2));
                if (it != hk->m_hash.end()) {
                    result = it->second;
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            if (String* sk = dynamic_cast<String*>(m_data[pattern])) {
                result = sk->m_string;
                return true;
            } else {
                return false;
            }
        }
    }
}
