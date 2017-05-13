/**
  *  \file arch/posix/posixdialog.cpp
  */

#if TARGET_OS_POSIX
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "arch/posix/posixdialog.hpp"
#include "arch/posix/posix.hpp"
#include "afl/string/string.hpp"

/*
  info:
      kdialog --msgbox <text> --title <title>
      gxmessage -wrap <text> -title <title>
      xmessage <text> -title <title>

  error:
      kdialog --error <text> --title <title>
      gxmessage -wrap <text> -title <title>
      xmessage <text> -title <title>

  yesno:
      kdialog --yesno <text> --title <title>
      gxmessage -buttons Yes:0,No:1 -wrap <text> -title <title>
      xmessage -buttons Yes:0,No:1 <text> -title <title>
 */

namespace {
    const char*const kdialog_info[]    = { "kdialog", "--msgbox", "I", "--title", "T", 0 };
    const char*const kdialog_error[]   = { "kdialog", "--error",  "I", "--title", "T", 0 };
    const char*const kdialog_yesno[]   = { "kdialog", "--yesno",  "I", "--title", "T", 0 };

    const char*const gxmessage_info[]  = { "gxmessage", "-wrap", "I", "-title", "T", 0 };
    const char*const gxmessage_yesno[] = { "gxmessage", "-buttons", "Yes:0,No:1", "-wrap", "I", "-title", "T", 0 };

    const char*const xmessage_info[]   = { "xmessage", "I", "-title", "T", 0 };
    const char*const xmessage_yesno[]  = { "xmessage", "-buttons", "Yes:0,No:1", "I", "-title", "T", 0 };

    const char*const*const info_commands[]  = { kdialog_info,  gxmessage_info,  xmessage_info, 0 };
    const char*const*const error_commands[] = { kdialog_error, gxmessage_info,  xmessage_info, 0 };
    const char*const*const yesno_commands[] = { kdialog_yesno, gxmessage_yesno, xmessage_yesno, 0 };

    const size_t MAX_ARGS = 20;


    void execTemplate(const char*const*const* templates, const String_t& sysInfo, const String_t& sysTitle)
    {
        while (const char*const* p = *templates++) {
            // Build command line
            const char* args[MAX_ARGS];
            size_t i = 0;
            while (i < MAX_ARGS-1 && p[i] != 0) {
                if (std::strcmp(p[i], "I") == 0) {
                    args[i] = sysInfo.c_str();
                } else if (std::strcmp(p[i], "T") == 0) {
                    args[i] = sysTitle.c_str();
                } else {
                    args[i] = p[i];
                }
                ++i;
            }
            args[i] = 0;

            // Execute the program. If this succeeds, this function will not return.
            execvp(p[0], const_cast<char**>(args));
        }
    }


    int doDialog(const char*const*const* templates,
                 const char* kind,
                 const String_t& info, const String_t& title)
    {
        // Convert messages to system character set
        String_t sysInfo;
        String_t sysTitle;
        arch::posix::convertUtf8ToExternal(sysInfo,  afl::string::toMemory(info), arch::posix::RelaxedConversion);
        arch::posix::convertUtf8ToExternal(sysTitle, afl::string::toMemory(title), arch::posix::RelaxedConversion);

        // Do we have reason to believe using an X11 message program will work?
        if (std::getenv("DISPLAY") != 0) {
            // Yes! Do so.
            pid_t child = fork();
            if (child >= 0) {
                if (child == 0) {
                    // We are the child.
                    execTemplate(templates, sysInfo, sysTitle);
                    _exit(42);
                } else {
                    // We are the parent
                    int status;
                    waitpid(child, &status, 0);
                    if (WIFEXITED(status)) {
                        if (WEXITSTATUS(status) != 42) {
                            return WEXITSTATUS(status);
                        }
                    } else {
                        return 1;
                    }
                }
            }
        }

        // Cannot run a graphical dialog
        std::printf("%s: %s\n", kind, sysInfo.c_str());
        return 1;
    }
}



void
arch::posix::PosixDialog::showInfo(String_t info, String_t title)
{
    doDialog(info_commands, "Info", info, title);
}

void
arch::posix::PosixDialog::showError(String_t info, String_t title)
{
    doDialog(error_commands, "Error", info, title);
}

bool
arch::posix::PosixDialog::askYesNo(String_t info, String_t title)
{
    return doDialog(yesno_commands, "Question", info, title) == 0;
}
#else
int g_variableToMakePosixDialogObjectFileNotEmpty;
#endif
