/**
  *  \file app/env.cpp
  *  \brief Sample Application: Environment
  */

#include <iostream>
#include "afl/sys/environment.hpp"

int main(int, char** argv)
{
    afl::sys::Environment& env = afl::sys::Environment::getInstance(argv);
    std::cout << "Settings directory: " << env.getSettingsDirectoryName("env") << "\n"
              << "Installation directory: " << env.getInstallationDirectoryName() << "\n"
              << "PATH environment: " << env.getEnvironmentVariable("PATH") << "\n"
              << "Command line:\n";

    afl::base::Ref<afl::sys::Environment::CommandLine_t> cmdl = env.getCommandLine();
    String_t s;
    while (cmdl->getNextElement(s)) {
        std::cout << "\t" << s << " (" << s.size() << "b)\n";
    }
    return 0;
}
