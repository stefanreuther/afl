/**
  *  \file afl/async/operation.cpp
  *  \brief Class afl::async::Operation
  */

#include <cassert>
#include "afl/async/operation.hpp"
#include "afl/async/cancelable.hpp"
#include "afl/async/notifier.hpp"

afl::async::Operation::Operation()
    : m_pController(0),
      m_pNotifier(&Notifier::getDefaultInstance())
{ }

afl::async::Operation::~Operation()
{ }
