/**
  *  \file arch/win32/win32dialog.hpp
  */
#ifndef AFL_ARCH_WIN32_WIN32DIALOG_HPP
#define AFL_ARCH_WIN32_WIN32DIALOG_HPP

#include "afl/string/string.hpp"
#include "afl/sys/dialog.hpp"

namespace arch { namespace win32 {

    class Win32Dialog : public afl::sys::Dialog {
     public:
        virtual void showInfo(String_t info, String_t title);
        virtual void showError(String_t info, String_t title);
        virtual bool askYesNo(String_t info, String_t title);
    };

} }

#endif
