/**
  *  \file arch/posix/posixenvironment.cpp
  */

#if TARGET_OS_POSIX
#include <cstdlib>
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include "arch/posix/posixenvironment.hpp"
#include "arch/posix/posixfilesystem.hpp"
#include "afl/string/char.hpp"
#include "arch/posix/posix.hpp"
#include "arch/posix/posixstream.hpp"
#include "afl/io/bufferedstream.hpp"
#include "afl/string/messages.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/string/posixfilenames.hpp"
#include "arch/posix/posixcwd.hpp"

namespace {
    /* CommandLine implementation for POSIX */
    class CommandLine : public afl::sys::Environment::CommandLine_t {
     public:
        CommandLine()
            : m_index(),
              m_commandLine()
            { }

        bool getNextElement(String_t& result)
            {
                if (m_index < m_commandLine.size()) {
                    result = m_commandLine[m_index++];
                    return true;
                } else {
                    return false;
                }
            }

        void add(const String_t& s)
            {
                m_commandLine.push_back(s);
            }

     private:
        std::size_t m_index;
        std::vector<String_t> m_commandLine;
    };



    /* Console implementation for POSIX.
       This is a stripped-down version of TextFile, with BOM-snooping removed and charset conversion hardcoded to convertExternalToUtf8.
       It must also keep the stream alive (Ptr<>). */
    class Console : public afl::io::TextReader, public afl::io::TextWriter {
     public:
        Console(afl::base::Ref<afl::io::Stream> s)
            : m_stream(s),
              m_buffer(*s)
            { }
        ~Console()
            { }

     protected:
        // TextReader:
        virtual bool doReadLine(String_t& line)
            {
                // Read line
                String_t rawLine;
                const uint8_t* pch;
                while ((pch = m_buffer.readByte()) != 0) {
                    if (*pch == 10) {
                        // LF, ends the line
                        break;
                    } else if (*pch == 13) {
                        // CR, could be CRLF; in any case, ends the line
                        pch = m_buffer.peekByte();
                        if (pch != 0 && *pch == 10) {
                            m_buffer.readByte();
                        }
                        break;
                    } else {
                        // Process it
                        rawLine.append(1, char(*pch));
                    }
                }

                // If the file ends in Ctrl-Z, remove that
                if (pch == 0 && rawLine.size() != 0 && rawLine[rawLine.size()-1] == char(26)) {
                    rawLine.erase(rawLine.size()-1);
                }

                // Produce result
                if (pch != 0 || rawLine.size() != 0) {
                    line = arch::posix::convertExternalToUtf8(afl::string::toMemory(rawLine));
                    return true;
                } else {
                    line.clear();
                    return false;
                }
            }

        // TextWriter:
        virtual void doWriteText(afl::string::ConstStringMemory_t data)
            {
                String_t tmp;
                arch::posix::convertUtf8ToExternal(tmp, data, arch::posix::RelaxedConversion);
                m_buffer.fullWrite(afl::string::toBytes(tmp));
            }

        virtual void doWriteNewline()
            {
                static const uint8_t NL[] = { '\n' };
                m_buffer.fullWrite(NL);
            }

        virtual void doFlush()
            {
                m_buffer.flush();
            }

     private:
        afl::base::Ref<afl::io::Stream> m_stream;
        afl::io::BufferedStream m_buffer;
    };
}

arch::posix::PosixEnvironment::PosixEnvironment(const char*const* argv)
    : m_argv(argv)
{ }

/* Get command line */
afl::base::Ref<afl::sys::Environment::CommandLine_t>
arch::posix::PosixEnvironment::getCommandLine()
{
    const char*const* argv = m_argv;
    afl::base::Ref<CommandLine> cmdl = *new CommandLine();
    while (const char* p = *++argv) {
        cmdl->add(convertExternalToUtf8(afl::string::toMemory(p)));
    }
    return cmdl;
}

/* Get invocation name */
String_t
arch::posix::PosixEnvironment::getInvocationName()
{
    return convertExternalToUtf8(afl::string::toMemory(m_argv[0]));
}

/* Get environment variable */
String_t
arch::posix::PosixEnvironment::getEnvironmentVariable(const String_t& name)
{
    // Convert name
    String_t u8Name;
    if (!convertUtf8ToExternal(u8Name, afl::string::toMemory(name), ParanoidConversion)) {
        return String_t();
    }

    // Query variable and convert result.
    return convertExternalToUtf8(afl::string::toMemory(::getenv(u8Name.c_str())));
}

/* Get settings folder name */
String_t
arch::posix::PosixEnvironment::getSettingsDirectoryName(const String_t& appName)
{
    String_t homeName = convertExternalToUtf8(afl::string::toMemory(::getenv("HOME")));
    if (homeName.empty()) {
        homeName = "/";
    }

    // Transform app name
    String_t transformedName = ".";
    bool upcase = false;
    for (size_t i = 0, n = appName.size(); i < n; ++i) {
        const char ch = appName[i];
        if (afl::string::charIsAlphanumeric(ch) || (ch == '.' && transformedName.size() > 1)) {
            transformedName += (upcase ? afl::string::charToUpper(ch) : afl::string::charToLower(ch));
            upcase = false;
        } else {
            // New word. Upcase if we actually have some previous text
            upcase = (transformedName.size() > 1);
        }
    }
    if (transformedName.size() > 1) {
        homeName = afl::string::PosixFileNames().makePathName(homeName, transformedName);
    }

    return homeName;
}

/* Get installation directory. */
String_t
arch::posix::PosixEnvironment::getInstallationDirectoryName()
{
    // File system instance
    afl::string::PosixFileNames fs;

    // Look up the name
    String_t exeName;
    bool exeNameOK = false;

    // Look for name using /proc filesystem
    afl::base::GrowableMemory<char> progName;
    if (readlinkWrap("/proc/self/exe", progName)) {
        // Null-terminate
        if (::access(afl::string::fromMemory(progName).c_str(), X_OK) == 0) {
            exeName = afl::string::fromMemory(progName);
            exeNameOK = true;
        }
    }

    // Look for name using argv
    if (!exeNameOK && m_argv && m_argv[0]) {
        const char* invocationName = m_argv[0];
        if (std::strchr(invocationName, '/') == 0) {
            // Need a path search
            const char* path = getenv("PATH");
            if (!path) {
                path = "/bin:/usr/bin";
            }

            while (path != 0) {
                // Build combined name
                String_t combinedName;
                if (const char* p = std::strchr(path, ':')) {
                    combinedName = fs.makePathName(String_t(path, p), invocationName);
                    path = p+1;
                } else {
                    combinedName = fs.makePathName(path, invocationName);
                    path = 0;
                }

                // Check success
                if (::access(combinedName.c_str(), X_OK) == 0) {
                    exeName = getAbsolutePathName(combinedName);
                    exeNameOK = true;
                    break;
                }
            }
        } else {
            // Absolute or relative path
            exeName = getAbsolutePathName(m_argv[0]);
            exeNameOK = true;
        }
    }

    // Found something?
    if (exeNameOK) {
        // We now have a name that looks absolute. It may, however, contain symlinks, as in
        //    $HOME/bin/myprog -> /opt/foo/bin/myprog
        // We only deal with file names as symlinks. The directory name can be a symlink if something like 'stow' is used.
        // In that case, we expect that if /usr/local/bin is a symlink to /opt/foo/bin, related directories have a symlink as well.
        // We have a hardcoded limit to avoid loops; 20 should be plenty.
        const int SYMLINK_LIMIT = 20;
        for (int i = 0; i < SYMLINK_LIMIT; ++i) {
            if (readlinkWrap(exeName.c_str(), progName)) {
                exeName = fs.makePathName(fs.getDirectoryName(exeName), afl::string::fromMemory(progName));
            } else {
                // Not a link -or- we cannot deal with it
                break;
            }
        }

        // Remove file name
        exeName = fs.getDirectoryName(exeName);

        // If final component is 'bin', remove that as well
        if (fs.getFileName(exeName) == "bin") {
            exeName = fs.getDirectoryName(exeName);
        }

        // Everything we did so far used the system character set. Convert to UTF-8.
        return convertExternalToUtf8(afl::string::toMemory(exeName));
    } else {
        // Fallback
        return ".";
    }
}

afl::string::LanguageCode
arch::posix::PosixEnvironment::getUserLanguage()
{
    const char* lang = std::getenv("LC_ALL");
    if (lang == 0) {
        lang = std::getenv("LC_MESSAGES");
    }
    if (lang == 0) {
        lang = std::getenv("LANG");
    }

    if (lang != 0) {
        return convertExternalToUtf8(afl::string::toMemory(lang));
    } else {
        return afl::string::LanguageCode();
    }
}

afl::base::Ref<afl::io::TextWriter>
arch::posix::PosixEnvironment::attachTextWriter(Channel ch)
{
    return *new Console(attachStream(ch));
}

afl::base::Ref<afl::io::TextReader>
arch::posix::PosixEnvironment::attachTextReader(Channel ch)
{
    return *new Console(attachStream(ch));
}

afl::base::Ref<afl::io::Stream>
arch::posix::PosixEnvironment::attachStream(Channel ch)
{
    switch (ch) {
     case Input:
        return *new PosixStream("<stdin>", 0);
     case Output:
        return *new PosixStream("<stdout>", 1);
     case Error:
        return *new PosixStream("<stderr>", 2);
    }
    throw afl::except::UnsupportedException("<attachStream>");
}

#else
int g_variableToMakePosixEnvironmentObjectFileNotEmpty;
#endif
