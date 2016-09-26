/**
  *  \file cxxtest/TestController.h
  *  \brief Test controller for CxxTest
  *
  *  This provides a minimal means of controlling which tests to run in CxxTest.
  *  It is implemented as a GUI listener because those get to see argc/argv without having to mess with CxxTest's internals.
  *
  *  Usage:
  *  - Compile testsuite with --gui=TestController
  *  - Run testsuite as
  *      ./testsuite -x -y -z       to omit x, y and z
  *      ./testsuite a              to run just a
  *      ./testsuite --list         to list the test names instead of running them
  *  where x,y,z,a are parts of test class names or file names.
  *  All parameters are "and"ed.
  */
#ifndef AFL_CXXTEST_TESTCONTROLLER_H
#define AFL_CXXTEST_TESTCONTROLLER_H

#include <cstdlib>
#include <cstdio>
#include <cxxtest/Gui.h>
#include <cstring>

namespace CxxTest {
    class TestController : public GuiListener {
     public:
        virtual void enterGui(int& argc, char** argv)
            {
                List& list = RealWorldDescription::suites();
                bool listOnly = false;
                for (SuiteDescription* desc = dynamic_cast<SuiteDescription*>(list.head()); desc != 0; desc = desc->next()) {
                    bool enable = true;

                    // Check whether the given parameter matches desc/file.
                    for (int i = 1; i < argc; ++i) {
                        const char* pattern = argv[i];
                        if (std::strcmp(pattern, "--list") == 0) {
                            listOnly = true;
                        } else {
                            bool negate = false;
                            if (pattern[0] == '-') {
                                ++pattern;
                                negate = true;
                            }
                            bool match = (std::strstr(desc->suiteName(), pattern) != 0
                                          || std::strstr(desc->file(), pattern) != 0);
                            if (match == negate) {
                                enable = false;
                                break;
                            }
                        }
                    }
                    desc->setActive(enable);

                    if (enable && listOnly) {
                        std::printf("%s [%s, %ux]\n", desc->suiteName(), desc->file(), desc->numTests());
                    }
                }
                if (listOnly) {
                    std::exit(0);
                }
            }
    };
}

#endif
