/**
  *  \file afl/io/filesystem.cpp
  *  \brief Class afl::io::FileSystem
  */

#include "afl/io/filesystem.hpp"
#include "arch/filesystem.hpp"

afl::io::FileSystem&
afl::io::FileSystem::getInstance()
{
    static FileSystem_t instance;
    return instance;
}
