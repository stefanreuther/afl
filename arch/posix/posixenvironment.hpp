/**
  *  \file arch/posix/posixenvironment.hpp
  */
#ifndef AFL_ARCH_POSIX_POSIXENVIRONMENT_HPP
#define AFL_ARCH_POSIX_POSIXENVIRONMENT_HPP

#include "afl/sys/environment.hpp"

namespace arch { namespace posix {

    class PosixEnvironment : public afl::sys::Environment {
     public:
        PosixEnvironment(const char*const* argv);

        virtual afl::base::Ref<afl::sys::Environment::CommandLine_t> getCommandLine();
        virtual String_t getInvocationName();
        virtual String_t getEnvironmentVariable(const String_t& name);
        virtual String_t getSettingsDirectoryName(const String_t& appName);
        virtual String_t getInstallationDirectoryName();
        virtual afl::base::Ref<afl::io::TextWriter> attachTextWriter(Channel ch);
        virtual afl::base::Ref<afl::io::TextReader> attachTextReader(Channel ch);
        virtual afl::base::Ref<afl::io::Stream> attachStream(Channel ch);
     private:
        const char*const* m_argv;
    };

} }

#endif
