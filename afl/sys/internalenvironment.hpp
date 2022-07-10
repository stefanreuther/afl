/**
  *  \file afl/sys/internalenvironment.hpp
  *  \brief Class afl::sys::InternalEnvironment
  */
#ifndef AFL_AFL_SYS_INTERNALENVIRONMENT_HPP
#define AFL_AFL_SYS_INTERNALENVIRONMENT_HPP

#include <map>
#include "afl/data/stringlist.hpp"
#include "afl/sys/environment.hpp"

namespace afl { namespace sys {

    /** Internal implementation of Environment.
        Provides an environment that is entirely runtime-configurable
        and does not depend on any properties of the actual runtime environment.

        This class is primarily intended for testing. */
    class InternalEnvironment : public Environment {
     public:
        /** Constructor.
            Make an empty environment. */
        InternalEnvironment();

        /** Destructor. */
        ~InternalEnvironment();

        /** Set command line.
            Provides result for getCommandLine().
            Default is an empty list.
            @param cmdl Command line. Will be copied. */
        void setCommandLine(const afl::data::StringList_t& cmdl);

        /** Set program invocation name.
            Provides result for getInvocationName().
            Default is empty.
            @param name Name */
        void setInvocationName(const String_t& name);

        /** Set environment variable value.
            Provides result for getEnvironmentVariable().
            Default is empty for every variable.
            @param name Name of environment variable; case-sensitive, ASCII/UTF-8.
            @param value Value of environment variable; UTF-8 */
        void setEnvironmentVariable(const String_t& name, const String_t& value);

        /** Set settings directory name.
            Provides result for getSettingsDirectoryName().
            Default is empty.
            @param name Name. Can contain a single "*" placeholder which is replaced by the appName parameter to getSettingsDirectoryName(). */
        void setSettingsDirectoryName(const String_t& name);

        /** Set installation directory name.
            Provides result for getInstallationDirectoryName().
            Default is empty.
            @param name Name */
        void setInstallationDirectoryName(const String_t& name);

        /** Set user language.
            Provides result for getUserLanguage().
            Default is empty (invalid language).
            @param code Language code */
        void setUserLanguage(afl::string::LanguageCode code);

        /** Set channel stream.
            The configured stream is returned as-is from attachStream().
            When attachTextWriter() or attachTextReader() is used, it is interpreted as a sequence of "\n"-delimited UTF-8 lines.

            If a stream is configured as null (default), the corresponding attach operation will fail with a FileProblemException.

            @param ch Channel
            @param stream Stream to attach, can be null */
        void setChannelStream(Channel ch, afl::base::Ptr<afl::io::Stream> stream);

        // Environment:
        virtual afl::base::Ref<CommandLine_t> getCommandLine();
        virtual String_t getInvocationName();
        virtual String_t getEnvironmentVariable(const String_t& name);
        virtual String_t getSettingsDirectoryName(const String_t& appName);
        virtual String_t getInstallationDirectoryName();
        virtual afl::string::LanguageCode getUserLanguage();
        virtual afl::base::Ref<afl::io::TextWriter> attachTextWriter(Channel ch);
        virtual afl::base::Ref<afl::io::TextReader> attachTextReader(Channel ch);
        virtual afl::base::Ref<afl::io::Stream> attachStream(Channel ch);

     private:
        afl::data::StringList_t m_commandLine;
        String_t m_invocationName;
        std::map<String_t, String_t> m_environment;
        String_t m_settingsDirectoryName;
        String_t m_installationDirectoryName;
        afl::string::LanguageCode m_userLanguage;
        afl::base::Ptr<afl::io::Stream> m_inputStream;
        afl::base::Ptr<afl::io::Stream> m_outputStream;
        afl::base::Ptr<afl::io::Stream> m_errorStream;
    };

} }

#endif
