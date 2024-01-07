/**
  *  \file test/afl/sys/dialogtest.cpp
  *  \brief Test for afl::sys::Dialog
  */

#include "afl/sys/dialog.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.sys.Dialog")
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
