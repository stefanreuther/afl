/**
  *  \file afl/sys/dialog.hpp
  *  \brief Interface afl::sys::Dialog
  */
#ifndef AFL_AFL_SYS_DIALOG_HPP
#define AFL_AFL_SYS_DIALOG_HPP

#include "afl/string/string.hpp"

namespace afl { namespace sys {

    /** Getting a user's attention.
        This interface provides a simple last-effort way to get a user's attention.
        Example uses include
        - a program running in the background that wants to show one or another status message
        - a program that failed to set up its real GUI

        The effort and quality this interface makes in getting the user's attention varies.

        - POSIX: tries kdialog, gxmessage, xmessage; otherwise, just prints a message to stdout.
        - Win32: uses the MessageBox standard dialog */
    class Dialog {
     public:
        /** Virtual destructor. */
        virtual ~Dialog();

        /** Show an information dialog.
            Use this to inform the user that something has been done successfully.
            \param info Information message. Should be self-contained.
            \param title Title. Should name the invoking program and may not be immediately visible to the user */
        virtual void showInfo(String_t info, String_t title) = 0;

        /** Show an error dialog.
            Use this to inform the user that something has failed.
            \param info Information message. Should be self-contained.
            \param title Title. Should name the invoking program and may not be immediately visible to the user */
        virtual void showError(String_t info, String_t title) = 0;

        /** Show a yes/no question dialog.
            Use this to ask the user a yes/no question.
            \param info Information message. Should be self-contained.
            \param title Title. Should name the invoking program and may not be immediately visible to the user
            \retval true User confirmed
            \retval false User declined, or asking the question failed */
        virtual bool askYesNo(String_t info, String_t title) = 0;

        /** Get system instance.
            Returns an instance of a Dialog descendant that interfaces the host's GUI. */
        static Dialog& getSystemInstance();
    };

} }

#endif
