/**
  *  \file afl/data/access.cpp
  *  \brief Class afl::data::Access
  */

#include "afl/data/access.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/scalarvalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/visitor.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/string/format.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/parse.hpp"
#include "afl/except/remoteerrorexception.hpp"

namespace {
    void invalidInteger()
    {
        throw afl::except::InvalidDataException(afl::string::Messages::invalidNumber());
    }

    class AccessVisitor : public afl::data::Visitor {
     public:
        virtual void visitError(const String_t& source, const String_t& str)
            { throw afl::except::RemoteErrorException(source, str); }
    };
}

afl::data::Access::Access(Value* p)
    : m_value(p)
{ }

afl::data::Access::Access(std::auto_ptr<Value>& p)
    : m_value(p.get())
{ }

int32_t
afl::data::Access::toInteger() const
{
    class Converter : public AccessVisitor {
     public:
        int32_t m_result;

        Converter()
            : m_result(0)
            { }

        virtual void visitString(const String_t& str)
            {
                if (str.empty()) {
                    m_result = 0;
                } else if (afl::string::strToInteger(str, m_result)) {
                    // ok
                } else {
                    invalidInteger();
                }
            }

        virtual void visitInteger(int32_t iv)
            { m_result = iv; }

        virtual void visitFloat(double fv)
            { m_result = int32_t(fv); }

        virtual void visitBoolean(bool bv)
            { m_result = bv; }

        virtual void visitHash(const Hash& /*hv*/)
            { invalidInteger(); }

        virtual void visitVector(const Vector& /*vv*/)
            { invalidInteger(); }

        virtual void visitOther(const Value& /*other*/)
            { invalidInteger(); }

        virtual void visitNull()
            { m_result = 0; }
    };
    Converter c;
    c.visit(m_value);
    return c.m_result;
}

String_t
afl::data::Access::toString() const
{
    class Converter : public AccessVisitor {
     public:
        String_t m_result;

        Converter()
            : m_result()
            { }

        virtual void visitString(const String_t& str)
            { m_result += str; }

        virtual void visitInteger(int32_t iv)
            { m_result += afl::string::Format("%d", iv); }

        virtual void visitFloat(double fv)
            { m_result += afl::string::Format("%.25g", fv); }

        virtual void visitBoolean(bool bv)
            { m_result += bv ? "true" : "false"; }

        virtual void visitHash(const Hash& /*hv*/)
            { m_result += "<hash>"; }

        virtual void visitVector(const Vector& vv)
            {
                for (size_t i = 0, n = vv.size(); i < n; ++i) {
                    if (i != 0) {
                        m_result += ",";
                    }
                    visit(vv.get(i));
                }
            }

        virtual void visitOther(const Value& /*other*/)
            { m_result += "<value>"; }

        virtual void visitNull()
            { }
    };
    Converter c;
    c.visit(m_value);
    return c.m_result;
}

size_t
afl::data::Access::getArraySize() const
{
    class Converter : public AccessVisitor {
     public:
        size_t m_result;

        Converter()
            : m_result(0)
            { }

        virtual void visitString(const String_t& /*str*/)
            { }

        virtual void visitInteger(int32_t /*iv*/)
            { }

        virtual void visitFloat(double /*fv*/)
            { }

        virtual void visitBoolean(bool /*bv*/)
            { }

        virtual void visitHash(const Hash& /*hv*/)
            { }

        virtual void visitVector(const Vector& vv)
            { m_result = vv.size(); }

        virtual void visitOther(const Value& /*other*/)
            { }

        virtual void visitNull()
            { }
    };
    Converter c;
    c.visit(m_value);
    return c.m_result;
}

void
afl::data::Access::getHashKeys(StringList_t& list) const
{
    class Converter : public AccessVisitor {
     public:
        StringList_t& m_result;

        Converter(StringList_t& result)
            : m_result(result)
            { }

        virtual void visitString(const String_t& /*str*/)
            { }

        virtual void visitInteger(int32_t /*iv*/)
            { }

        virtual void visitFloat(double /*fv*/)
            { }

        virtual void visitBoolean(bool /*bv*/)
            { }

        virtual void visitHash(const Hash& hv)
            {
                const NameMap& names = hv.getKeys();
                for (NameMap::Index_t i = 0, n = names.getNumNames(); i < n; ++i) {
                    m_result.push_back(names.getNameByIndex(i));
                }
            }

        virtual void visitVector(const Vector& vv)
            {
                for (size_t i = 0, n = vv.size(); i+1 < n; i += 2) {
                    m_result.push_back(Access(vv.get(i)).toString());
                }
            }

        virtual void visitOther(const Value& /*other*/)
            { }

        virtual void visitNull()
            { }
    };
    Converter c(list);
    c.visit(m_value);
}

afl::data::Access
afl::data::Access::operator[](size_t index) const
{
    class Converter : public AccessVisitor {
     public:
        Value* m_result;
        size_t m_index;

        Converter(size_t index)
            : m_result(0),
              m_index(index)
            { }

        virtual void visitString(const String_t& /*str*/)
            { }

        virtual void visitInteger(int32_t /*iv*/)
            { }

        virtual void visitFloat(double /*fv*/)
            { }

        virtual void visitBoolean(bool /*bv*/)
            { }

        virtual void visitHash(const Hash& /*hv*/)
            { }

        virtual void visitVector(const Vector& vv)
            { m_result = vv.get(m_index); }

        virtual void visitOther(const Value& /*other*/)
            { }

        virtual void visitNull()
            { }
    };
    Converter c(index);
    c.visit(m_value);
    return c.m_result;
}

afl::data::Access
afl::data::Access::operator()(String_t key) const
{
    class Converter : public AccessVisitor {
     public:
        Value* m_result;
        const String_t& m_key;

        Converter(const String_t& key)
            : m_result(0),
              m_key(key)
            { }

        virtual void visitString(const String_t& /*str*/)
            { }

        virtual void visitInteger(int32_t /*iv*/)
            { }

        virtual void visitFloat(double /*fv*/)
            { }

        virtual void visitBoolean(bool /*bv*/)
            { }

        virtual void visitHash(const Hash& hv)
            {
                m_result = hv.get(m_key);
            }

        virtual void visitVector(const Vector& vv)
            {
                for (size_t i = 0, n = vv.size(); i+1 < n; i += 2) {
                    if (Access(vv.get(i)).toString() == m_key) {
                        m_result = vv.get(i+1);
                        break;
                    }
                }
            }

        virtual void visitOther(const Value& /*other*/)
            { }

        virtual void visitNull()
            { }
    };
    Converter c(key);
    c.visit(m_value);
    return c.m_result;
}

afl::data::Access
afl::data::Access::operator()(const char* key) const
{
    return operator()(String_t(key));
}

afl::data::Value*
afl::data::Access::getValue() const
{
    return m_value;
}

void
afl::data::Access::toIntegerList(IntegerList_t& list) const
{
    // Simple implementation. If speed becomes a concern, implement a custom visitor here.
    for (size_t i = 0, n = getArraySize(); i < n; ++i) {
        list.push_back((*this)[i].toInteger());
    }
}

void
afl::data::Access::toStringList(StringList_t& list) const
{
    // Simple implementation. If speed becomes a concern, implement a custom visitor here.
    for (size_t i = 0, n = getArraySize(); i < n; ++i) {
        list.push_back((*this)[i].toString());
    }
}

bool
afl::data::Access::isNull() const
{
    return m_value == 0;
}

bool
afl::data::Access::isError(String_t& source, String_t& str) const
{
    class Converter : public afl::data::Visitor {
     public:
        String_t& m_source;
        String_t& m_str;
        bool m_result;

        Converter(String_t& source, String_t& str)
            : m_source(source),
              m_str(str),
              m_result(false)
            { }

        virtual void visitString(const String_t& /*str*/)
            { }

        virtual void visitInteger(int32_t /*iv*/)
            { }

        virtual void visitFloat(double /*fv*/)
            { }

        virtual void visitBoolean(bool /*bv*/)
            { }

        virtual void visitHash(const Hash& /*hv*/)
            { }

        virtual void visitVector(const Vector& /*vv*/)
            { }

        virtual void visitOther(const Value& /*other*/)
            { }

        virtual void visitError(const String_t& source, const String_t& str)
            {
                m_source = source;
                m_str = str;
                m_result = true;
            }

        virtual void visitNull()
            { }
    };
    Converter c(source, str);
    c.visit(m_value);
    return c.m_result;
}
