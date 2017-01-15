/**
  *  \file u/t_sys_dialog.cpp
  *  \brief Test for afl::sys::Dialog
  */

#include "afl/sys/dialog.hpp"

#include "t_sys.hpp"

/** Interface test. */
void
TestSysDialog::testIt()
{
    // Test the interface
    class Tester : public afl::sys::Dialog {
     public:
        virtual void showInfo(String_t /*info*/, String_t /*title*/)
            { }
        virtual void showError(String_t /*info*/, String_t /*title*/)
            { }
        virtual bool askYesNo(String_t /*info*/, String_t /*title*/)
            { return false; }
    };
    Tester t;

    // Test that we can access the system instance
    afl::sys::Dialog::getSystemInstance();
}

