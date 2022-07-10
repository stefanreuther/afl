/**
  *  \file afl/sys/internalenvironment.cpp
  *  \brief Class afl::sys::InternalEnvironment
  */

#include "afl/sys/internalenvironment.hpp"
#include "afl/charset/utf8charset.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/textfile.hpp"
#include "afl/string/messages.hpp"

afl::sys::InternalEnvironment::InternalEnvironment()
    : m_commandLine(),
      m_invocationName(),
      m_environment(),
      m_settingsDirectoryName(),
      m_installationDirectoryName(),
      m_userLanguage(),
      m_inputStream(),
      m_outputStream(),
      m_errorStream()
{ }

afl::sys::InternalEnvironment::~InternalEnvironment()
{ }

void
afl::sys::InternalEnvironment::setCommandLine(const afl::data::StringList_t& cmdl)
{
    m_commandLine = cmdl;
}

void
afl::sys::InternalEnvironment::setInvocationName(const String_t& name)
{
    m_invocationName = name;
}

void
afl::sys::InternalEnvironment::setEnvironmentVariable(const String_t& name, const String_t& value)
{
    m_environment[name] = value;
}

void
afl::sys::InternalEnvironment::setSettingsDirectoryName(const String_t& name)
{
    m_settingsDirectoryName = name;
}

void
afl::sys::InternalEnvironment::setInstallationDirectoryName(const String_t& name)
{
    m_installationDirectoryName = name;
}

void
afl::sys::InternalEnvironment::setUserLanguage(afl::string::LanguageCode code)
{
    m_userLanguage = code;
}

void
afl::sys::InternalEnvironment::setChannelStream(Channel ch, afl::base::Ptr<afl::io::Stream> stream)
{
    switch (ch) {
     case Input:  m_inputStream  = stream; break;
     case Output: m_outputStream = stream; break;
     case Error:  m_errorStream  = stream; break;
    }
}

afl::base::Ref<afl::sys::Environment::CommandLine_t>
afl::sys::InternalEnvironment::getCommandLine()
{
    class Enum : public afl::sys::Environment::CommandLine_t {
     public:
        Enum(const afl::data::StringList_t& list)
            : m_list(list), m_pos(0)
            { }
        bool getNextElement(String_t& out)
            {
                if (m_pos < m_list.size()) {
                    out = m_list[m_pos++];
                    return true;
                } else {
                    return false;
                }
            }
     private:
        afl::data::StringList_t m_list;
        size_t m_pos;
    };
    return *new Enum(m_commandLine);
}

String_t
afl::sys::InternalEnvironment::getInvocationName()
{
    return m_invocationName;
}

String_t
afl::sys::InternalEnvironment::getEnvironmentVariable(const String_t& name)
{
    std::map<String_t, String_t>::const_iterator it = m_environment.find(name);
    if (it != m_environment.end()) {
        return it->second;
    } else {
        return String_t();
    }
}

String_t
afl::sys::InternalEnvironment::getSettingsDirectoryName(const String_t& appName)
{
    String_t result = m_settingsDirectoryName;
    String_t::size_type p = result.rfind('*');
    if (p != String_t::npos) {
        result.replace(p, 1, appName);
    }
    return result;
}

String_t
afl::sys::InternalEnvironment::getInstallationDirectoryName()
{
    return m_installationDirectoryName;
}

afl::string::LanguageCode
afl::sys::InternalEnvironment::getUserLanguage()
{
    return m_userLanguage;
}

afl::base::Ref<afl::io::TextWriter>
afl::sys::InternalEnvironment::attachTextWriter(Channel ch)
{
    // Unfortunately, TextFile indirectly derives from RefCounted multiple times, and thus cannot be used with Ref. D'oh.
    // Therefore, we need to explicitly make an instance of TextWriter (and TextReader, below) to forward methods.
    // (A forwarder object is required anyway to keep the Ref<Stream> alive.)
    class TextWriterWithStream : public afl::io::TextWriter {
     public:
        TextWriterWithStream(afl::base::Ref<afl::io::Stream> stream)
            : TextWriter(),
              m_stream(stream),
              m_textFile(*stream)
            {
                m_textFile.setSystemNewline(false);
                m_textFile.setUtf8Snoop(false);
                m_textFile.setCharsetNew(new afl::charset::Utf8Charset());
            }
        ~TextWriterWithStream()
            { m_textFile.flush(); }
        virtual void doWriteText(afl::string::ConstStringMemory_t data)
            { m_textFile.writeText(data); }
        virtual void doWriteNewline()
            { m_textFile.writeLine(); }
        virtual void doFlush()
            { m_textFile.flush(); }
     private:
        afl::base::Ref<afl::io::Stream> m_stream;
        afl::io::TextFile m_textFile;
    };
    return *new TextWriterWithStream(attachStream(ch));
}

afl::base::Ref<afl::io::TextReader>
afl::sys::InternalEnvironment::attachTextReader(Channel ch)
{
    class TextReaderWithStream : public afl::io::TextReader {
     public:
        TextReaderWithStream(afl::base::Ref<afl::io::Stream> stream)
            : TextReader(),
              m_stream(stream),
              m_textFile(*stream)
            {
                m_textFile.setSystemNewline(false);
                m_textFile.setUtf8Snoop(false);
                m_textFile.setCharsetNew(new afl::charset::Utf8Charset());
            }
        ~TextReaderWithStream()
            { }
        virtual bool doReadLine(String_t& out)
            { return m_textFile.readLine(out); }
     private:
        afl::base::Ref<afl::io::Stream> m_stream;
        afl::io::TextFile m_textFile;
    };
    return *new TextReaderWithStream(attachStream(ch));
}

afl::base::Ref<afl::io::Stream>
afl::sys::InternalEnvironment::attachStream(Channel ch)
{
    afl::base::Ptr<afl::io::Stream> result;
    switch (ch) {
     case Input:  result = m_inputStream;  break;
     case Output: result = m_outputStream; break;
     case Error:  result = m_errorStream;  break;
    }
    if (result.get() == 0) {
        throw afl::except::FileProblemException("<attachStream>", afl::string::Messages::invalidOperation());
    }
    return *result;
}
