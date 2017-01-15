/**
  *  \file afl/sys/dialog.cpp
  *  \brief Interface afl::sys::Dialog
  */

#include "afl/sys/dialog.hpp"
#include "arch/dialog.hpp"

afl::sys::Dialog::~Dialog()
{ }

afl::sys::Dialog&
afl::sys::Dialog::getSystemInstance()
{
    static Dialog_t instance;
    return instance;
}
