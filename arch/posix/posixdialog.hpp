/**
  *  \file arch/posix/posixdialog.hpp
  */
#ifndef AFL_ARCH_POSIX_POSIXDIALOG_HPP
#define AFL_ARCH_POSIX_POSIXDIALOG_HPP

#include "afl/string/string.hpp"
#include "afl/sys/dialog.hpp"

namespace arch { namespace posix {

    class PosixDialog : public afl::sys::Dialog {
     public:
        virtual void showInfo(String_t info, String_t title);
        virtual void showError(String_t info, String_t title);
        virtual bool askYesNo(String_t info, String_t title);
    };

} }

#endif
