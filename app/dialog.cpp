/**
  *  \file app/dialog.cpp
  *  \brief Sample Application: Dialogs
  */

#include <iostream>
#include "afl/base/ref.hpp"
#include "afl/sys/environment.hpp"
#include "afl/base/optional.hpp"
#include "afl/sys/dialog.hpp"

int main(int, char** argv)
{
    using afl::sys::Environment;
    using afl::base::Ref;
    using afl::base::Optional;

    // Get environment
    Ref<Environment::CommandLine_t> cmdl = Environment::getInstance(argv).getCommandLine();

    // Args
    enum Mode { Info, Error, YesNo } mode = Info;
    Optional<String_t> text;
    Optional<String_t> title;

    // Parse args
    String_t arg;
    while (cmdl->getNextElement(arg)) {
        if (arg == "-info") {
            mode = Info;
        } else if (arg == "-error") {
            mode = Error;
        } else if (arg == "-yesno") {
            mode = YesNo;
        } else if (!text.isValid()) {
            text = arg;
        } else if (!title.isValid()) {
            title = arg;
        } else {
            std::cerr << "Argument not understood: " << arg << "\n";
            return 1;
        }
    }

    // Do it
    switch (mode) {
     case Info:
        afl::sys::Dialog::getSystemInstance().showInfo(text.orElse("<no text>"), title.orElse("Info"));
        break;

     case Error:
        afl::sys::Dialog::getSystemInstance().showError(text.orElse("<no text>"), title.orElse("Error"));
        break;

     case YesNo:
        if (afl::sys::Dialog::getSystemInstance().askYesNo(text.orElse("<no text>"), title.orElse("YesNo"))) {
            std::cout << "Confirmed.\n";
        } else {
            std::cout << "Declined.\n";
        }
        break;
    }
}
