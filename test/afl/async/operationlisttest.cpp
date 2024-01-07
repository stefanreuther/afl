/**
  *  \file test/afl/async/operationlisttest.cpp
  *  \brief Test for afl::async::OperationList
  */

#include "afl/async/operationlist.hpp"

#include "afl/async/controller.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/test/testrunner.hpp"

/** Test all operations with the base class. */
AFL_TEST("afl.async.OperationList:base", a)
{
    afl::async::OperationList<afl::async::Operation> list;
    afl::async::Operation op1, op2, op3;
    afl::async::Controller ctl;

    // Nothing there yet
    a.check("01. extractFront", list.extractFront() == 0);
    a.check("02. front", list.front() == 0);
    a.check("03. empty", list.empty());
    a.check("04. extractByController", list.extractByController(&ctl) == 0);
    a.check("05. remove", !list.remove(&op1));

    // Move two items through the list
    list.pushBack(&op1);
    list.pushBack(&op2);
    a.check("11. empty", !list.empty());
    a.check("12. front", list.front() == &op1);
    a.check("13. extractFront", list.extractFront() == &op1);
    a.check("14. empty", !list.empty());
    a.check("15. front", list.front() == &op2);
    a.check("16. extractFront", list.extractFront() == &op2);
    a.check("17. empty", list.empty());
    a.check("18. front", list.front() == 0);
    a.check("19. extractFront", list.extractFront() == 0);

    // Now with remove()
    list.pushBack(&op1);
    list.pushBack(&op2);
    a.check("21. empty", !list.empty());
    a.check("22. front", list.front() == &op1);

    a.check("23. remove", list.remove(&op2));
    a.check("24. remove", !list.remove(&op2));

    a.check("25. empty", !list.empty());
    a.check("26. front", list.front() == &op1);
    a.check("27. extractFront", list.extractFront() == &op1);
    a.check("28. empty", list.empty());
    a.check("29. front", list.front() == 0);
    a.check("2A. extractFront", list.extractFront() == 0);

    // Now with controller
    op1.setController(&ctl);
    op3.setController(&ctl);
    list.pushBack(&op1);
    list.pushBack(&op2);
    list.pushBack(&op3);
    a.check("31. extractByController", list.extractByController(&ctl) == &op1);
    a.check("32. extractByController", list.extractByController(&ctl) == &op3);
    a.check("33. extractByController", list.extractByController(&ctl) == 0);

    a.check("34. empty", !list.empty());
    a.check("35. front", list.front() == &op2);
    a.check("36. extractFront", list.extractFront() == &op2);
    a.check("37. empty", list.empty());
    a.check("38. front", list.front() == 0);
    a.check("39. extractFront", list.extractFront() == 0);
}

/** Test all operations with a derived class. */
AFL_TEST("afl.async.OperationList:derived", a)
{
    afl::async::OperationList<afl::async::SendOperation> list;
    afl::async::SendOperation op1;
    afl::async::Controller ctl;

    // Nothing there yet
    a.check("01. extractFront", list.extractFront() == 0);
    a.check("02. front", list.front() == 0);
    a.check("03. empty", list.empty());
    a.check("04. extractByController", list.extractByController(&ctl) == 0);
    a.check("05. remove", !list.remove(&op1));

    // Walk element through the list
    list.pushBack(&op1);

    a.check("11. empty", !list.empty());
    a.check("12. front", list.front() == &op1);
    a.check("13. extractFront", list.extractFront() == &op1);
    a.check("14. empty", list.empty());
    a.check("15. front", list.front() == 0);
    a.check("16. extractFront", list.extractFront() == 0);
}
