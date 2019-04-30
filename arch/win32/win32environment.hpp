/**
  *  \file arch/win32/win32environment.hpp
  */
#ifndef AFL_ARCH_WIN32_WIN32ENVIRONMENT_HPP
#define AFL_ARCH_WIN32_WIN32ENVIRONMENT_HPP

#include "afl/sys/environment.hpp"

namespace arch { namespace win32 {

    class Win32Environment : public afl::sys::Environment {
     public:
        Win32Environment(const char*const* argv);

        virtual afl::base::Ref<afl::sys::Environment::CommandLine_t> getCommandLine();
        virtual String_t getInvocationName();
        virtual String_t getEnvironmentVariable(const String_t& name);
        virtual String_t getSettingsDirectoryName(const String_t& appName);
        virtual String_t getInstallationDirectoryName();
        virtual afl::string::LanguageCode getUserLanguage();
        virtual afl::base::Ref<afl::io::TextWriter> attachTextWriter(Channel ch);
        virtual afl::base::Ref<afl::io::TextReader> attachTextReader(Channel ch);
        virtual afl::base::Ref<afl::io::Stream> attachStream(Channel ch);

     private:
        String_t m_programName;
    };

} }

#endif
