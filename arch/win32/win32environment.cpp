/**
  *  \file arch/win32/win32environment.cpp
  */

#ifdef TARGET_OS_WIN32
#include <windows.h>
#include <shlobj.h>
#include <vector>
#include <stdexcept>
#include "arch/win32/win32environment.hpp"
#include "arch/win32/win32.hpp"
#include "arch/win32/win32filesystem.hpp"
#include "arch/win32/win32stream.hpp"
#include "afl/io/bufferedstream.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/charset/utf8.hpp"

namespace {
    /* CommandLine implementation for Win32 */
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

    HANDLE getChannelHandle(arch::win32::Win32Environment::Channel ch)
    {
        switch (ch) {
        case arch::win32::Win32Environment::Input:
            return GetStdHandle(STD_INPUT_HANDLE);
        case arch::win32::Win32Environment::Output:
            return GetStdHandle(STD_OUTPUT_HANDLE);
        case arch::win32::Win32Environment::Error:
            return GetStdHandle(STD_ERROR_HANDLE);
        }
        return NULL;
    }

    const char* getChannelName(arch::win32::Win32Environment::Channel ch)
    {
        switch (ch) {
        case arch::win32::Win32Environment::Input:
            return "<stdin>";
        case arch::win32::Win32Environment::Output:
            return "<stdout>";
        case arch::win32::Win32Environment::Error:
            return "<stderr>";
        }
        return "?";
    }
}

arch::win32::Win32Environment::Win32Environment(const char*const* /*argv*/)
{ }

/* Get command line */
afl::base::Ptr<afl::sys::Environment::CommandLine_t>
arch::win32::Win32Environment::getCommandLine()
{
    afl::base::Ptr<CommandLine> cmdl = new CommandLine();

    // Unicode command line is supported on all Windows versions.
    // Parse it into argv.
    static const wchar_t SPACE = L' ', TAB = L'\t', NUL = L'\0', QUOTE = L'"';
    const wchar_t* p = GetCommandLineW();
    bool first = true;
    afl::charset::Utf8 u8(0);
    while (1) {
        // Skip whitespace
        while (*p == SPACE || *p == TAB) {
            ++p;
        }
        if (*p == NUL) {
            break;
        }

        // Gather a word
        String_t word;
        if (*p == QUOTE) {
            // Quoted
            ++p;
            while (*p != NUL) {
                wchar_t ch = *p++;
                if (ch == QUOTE) {
                    if (*p == QUOTE) {
                        u8.append(word, QUOTE);
                        ++p;
                    } else {
                        break;
                    }
                } else {
                    u8.append(word, ch);
                }
            }
        } else {
            // Simple word
            while (*p != SPACE && *p != TAB && *p != NUL) {
                u8.append(word, *p);
                ++p;
            }
        }
        if (!first) {
            cmdl->add(word);
        } else {
            first = false;
        }
    }
    return cmdl;
}

/* Get environment variable */
String_t
arch::win32::Win32Environment::getEnvironmentVariable(const String_t& name)
{
    if (hasUnicodeSupport()) {
        // Unicode version
        WStr uniName;
        convertToUnicode(uniName, afl::string::toMemory(name));
        uniName.push_back(L'\0');

        DWORD n = GetEnvironmentVariableW(&uniName[0], 0, 0);
        if (n != 0) {
            WStr value(n+1);
            value.resize(GetEnvironmentVariableW(&uniName[0], &value[0], value.size()));
            return convertFromUnicode(value);
        } else {
            return String_t();
        }
    } else {
        // ANSI version
        String_t ansiName(convertToANSI(afl::string::toMemory(name)));
        DWORD n = GetEnvironmentVariableA(ansiName.c_str(), 0, 0);
        if (n != 0) {
            std::vector<char> value(n+1);
            value.resize(GetEnvironmentVariableA(ansiName.c_str(), &value[0], value.size()));
            return convertFromANSI(value);
        } else {
            return String_t();
        }
    }
}

/* Get settings folder name */
String_t
arch::win32::Win32Environment::getSettingsDirectoryName(const String_t& appName)
{
    String_t name = arch::win32::getShellFolder(CSIDL_APPDATA);
    if (name.empty()) {
        name = "C:\\";
    }
    if (!appName.empty()) {
        name = arch::win32::Win32FileSystem().makePathName(name, appName);
    }
    return name;
}

String_t
arch::win32::Win32Environment::getInstallationDirectoryName()
{
    const size_t N = 1024;
    if (hasUnicodeSupport()) {
        wchar_t buffer[N];
        int len = GetModuleFileNameW(NULL, buffer, N);
        if (len >= 0) {
            return Win32FileSystem().getDirectoryName(convertFromUnicode(afl::base::Memory<wchar_t>::unsafeCreate(buffer, len)));
        } else {
            return String_t();
        }
    } else {
        char buffer[N];
        int len = GetModuleFileNameA(NULL, buffer, N);
        if (len >= 0) {
            return Win32FileSystem().getDirectoryName(convertFromANSI(afl::base::Memory<char>::unsafeCreate(buffer, len)));
        } else {
            return String_t();
        }
    }
}

namespace {
    /* Console writer implementation for ANSI console.
       Note that this does NOT close the handle because it might be shared. */
    class ConsoleWriterA : public afl::io::TextWriter {
     public:
        ConsoleWriterA(HANDLE h)
            : m_handle(h),
              m_codepage(GetConsoleOutputCP())
            { }

     protected:
        // TextWriter:
        virtual void doWriteText(afl::string::ConstStringMemory_t data)
            {
                String_t text = arch::win32::convertToCodepage(data, m_codepage);
                DWORD written;
                if (!text.empty()) {
                    WriteConsoleA(m_handle, text.data(), text.size(), &written, 0);
                }
            }

        virtual void doWriteNewline()
            {
                const char NL[] = {'\r','\n'};
                DWORD written;
                WriteConsoleA(m_handle, NL, sizeof(NL), &written, 0);
            }

        virtual void doFlush()
            { }

     private:
        HANDLE m_handle;
        UINT m_codepage;
    };

    /* Console writer implementation for Unicode console
       Note that this does NOT close the handle because it might be shared. */
    class ConsoleWriterW : public afl::io::TextWriter {
    public:
        ConsoleWriterW(HANDLE h)
            : m_handle(h)
            { }

     protected:
        // TextWriter:
        virtual void doWriteText(afl::string::ConstStringMemory_t data)
            {
                arch::win32::WStr out;
                arch::win32::convertToUnicode(out, data);
                DWORD written;
                if (!out.empty()) {
                    WriteConsoleW(m_handle, &out[0], out.size(), &written, 0);
                }
            }

        virtual void doWriteNewline()
            {
                const wchar_t NL[] = {L'\r', L'\n'};
                DWORD written;
                WriteConsoleW(m_handle, NL, sizeof(NL)/sizeof(NL[0]), &written, 0);
            }

        virtual void doFlush()
            { }
     private:
        HANDLE m_handle;
    };

    /* Console reader implementation for ANSI console.
       Note that this does NOT close the handle because it might be shared. */
    class ConsoleReaderA : public afl::io::TextReader {
     public:
        ConsoleReaderA(HANDLE h)
            : m_handle(h),
              m_codepage(GetConsoleOutputCP()),
              m_buffer()
            { }

     protected:
        virtual bool doReadLine(String_t& line)
            {
                String_t tmp;
                bool eof = false;
                while (1) {
                    const char* p = m_buffer.eat();
                    if (p == 0) {
                        // Read more characters
                        DWORD read = 0;
                        if (ReadConsoleA(m_handle, m_bufferData, sizeof(m_bufferData), &read, 0) && read != 0) {
                            m_buffer = afl::base::Memory<const char>::unsafeCreate(m_bufferData, read);
                        } else {
                            // End reached
                            eof = true;
                            break;
                        }
                    } else {
                        // Got a character; process it
                        if (*p == '\r') {
                            // ignore
                        } else if (*p == '\n') {
                            // finish
                            return true;
                        } else if (*p == 26) {
                            // Ctrl-Z
                            eof = true;
                            break;
                        } else {
                            tmp += *p;
                        }
                    }
                }

                // Convert
                if (tmp.empty() && eof) {
                    line.clear();
                    return false;
                } else {
                    line = arch::win32::convertFromCodepage(afl::string::toMemory(tmp), m_codepage);
                    return true;
                }
            }

     private:
        HANDLE m_handle;
        UINT m_codepage;
        char m_bufferData[1024];
        afl::base::Memory<const char> m_buffer;
    };

    /* Console reader implementation for Unicode console.
       Note that this does NOT close the handle because it might be shared. */
    class ConsoleReaderW : public afl::io::TextReader {
     public:
        ConsoleReaderW(HANDLE h)
            : m_handle(h),
              m_buffer()
            { }

     protected:
        virtual bool doReadLine(String_t& line)
            {
                arch::win32::WStr tmp;
                bool eof = false;
                while (1) {
                    wchar_t* p = m_buffer.eat();
                    if (p == 0) {
                        // Read more characters
                        DWORD read = 0;
                        if (ReadConsoleW(m_handle, m_bufferData, sizeof(m_bufferData)/sizeof(m_bufferData[0]), &read, 0) && read != 0) {
                            m_buffer = afl::base::Memory<wchar_t>::unsafeCreate(m_bufferData, read);
                        } else {
                            // End reached
                            eof = true;
                            break;
                        }
                    } else {
                        // Got a character; process it
                        if (*p == L'\r') {
                            // ignore
                        } else if (*p == L'\n') {
                            // finish
                            return true;
                        } else if (*p == 26) {
                            // Ctrl-Z
                            eof = true;
                            break;
                        } else {
                            tmp.push_back(*p);
                        }
                    }
                }

                // Convert
                if (tmp.empty() && eof) {
                    line.clear();
                    return false;
                } else {
                    line = arch::win32::convertFromUnicode(tmp);
                    return true;
                }
            }

     private:
        HANDLE m_handle;
        wchar_t m_bufferData[1024];
        afl::base::Memory<wchar_t> m_buffer;
    };

    /* Console implemented using a (Buffered)Stream.
       This is a stripped-down version of TextFile that just writes UTF-8.
       It must also keep the stream alive (Ptr<>). */
    class ConsoleStream : public afl::io::TextReader, public afl::io::TextWriter {
     public:
        ConsoleStream(afl::base::Ptr<afl::io::Stream> s)
            : m_stream(s),
              m_buffer(*s)
            { }
        ~ConsoleStream()
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
                    rawLine.swap(line);
                    return true;
                } else {
                    line.clear();
                    return false;
                }
            }

        // TextWriter:
        virtual void doWriteText(afl::string::ConstStringMemory_t data)
            { m_buffer.fullWrite(data.toBytes()); }

        virtual void doWriteNewline()
            {
                static const uint8_t NL[] = { '\r', '\n' };
                m_buffer.fullWrite(NL);
            }

        virtual void doFlush()
            { m_buffer.flush(); }

     private:
        afl::base::Ptr<afl::io::Stream> m_stream;
        afl::io::BufferedStream m_buffer;
    };

}

afl::base::Ptr<afl::io::TextWriter>
arch::win32::Win32Environment::attachTextWriter(Channel ch)
{
    DWORD tmp;
    HANDLE h = getChannelHandle(ch);
    if (h == NULL || h == INVALID_HANDLE_VALUE) {
        // GUI app with no standard handle
        // This is the same case as for a POSIX app invoked with a closed handle (">&-").
        throw std::runtime_error(afl::string::Messages::invalidOperation());
    } else if (GetConsoleMode(h, &tmp)) {
        // Console
        if (hasUnicodeSupport()) {
            return new ConsoleWriterW(h);
        } else {
            return new ConsoleWriterA(h);
        }
    } else {
        // File
        return new ConsoleStream(attachStream(ch));
    }
}

afl::base::Ptr<afl::io::TextReader>
arch::win32::Win32Environment::attachTextReader(Channel ch)
{
    DWORD tmp;
    HANDLE h = getChannelHandle(ch);
    if (h == NULL || h == INVALID_HANDLE_VALUE) {
        // GUI app with no standard handle
        // This is the same case as for a POSIX app invoked with a closed handle (">&-").
        throw std::runtime_error(afl::string::Messages::invalidOperation());
    } else if (GetConsoleMode(h, &tmp)) {
        // Console
        if (hasUnicodeSupport()) {
            return new ConsoleReaderW(h);
        } else {
            return new ConsoleReaderA(h);
        }
    } else {
        // File
        return new ConsoleStream(attachStream(ch));
    }
}

afl::base::Ptr<afl::io::Stream>
arch::win32::Win32Environment::attachStream(Channel ch)
{
    HANDLE h = getChannelHandle(ch);
    if (h == NULL || h == INVALID_HANDLE_VALUE) {
        // GUI app with no standard handle
        // This is the same case as for a POSIX app invoked with a closed handle (">&-").
        throw std::runtime_error(afl::string::Messages::invalidOperation());
    } else {
        // Duplicate handle to get our own copy
        const char* name = getChannelName(ch);
        HANDLE newH;
        if (!DuplicateHandle(GetCurrentProcess(), h, GetCurrentProcess(), &newH, 0, FALSE /*don't inherit*/, DUPLICATE_SAME_ACCESS)) {
            throw afl::except::FileSystemException(name, afl::sys::Error::current());
        }

        // Return a stream
        return new Win32Stream(name, newH);
    }
}

#else
int g_variableToMakeWin32EnvironmentObjectFileNotEmpty;
#endif
