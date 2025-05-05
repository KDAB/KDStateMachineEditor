/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "elementwalker.h"
#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QVector>
#include <QTest>

using namespace KDSME;

class UtilTest : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void testElementWalker();
};

void UtilTest::testElementWalker()
{
    // returns list of walked states
    auto walkAllStates = [](ElementWalker &&walker, Element *item) -> QVector<Element *> {
        QVector<Element *> walkedStates;
        walker.walkItems(item, [&](Element *state) -> ElementWalker::VisitResult {
            walkedStates << state;
            return ElementWalker::RecursiveWalk;
        });
        return walkedStates;
    };

    // returns number of calls to our visit-lambda
    auto walkOneState = [](ElementWalker &&walker, Element *item) -> int {
        int counter = -1;
        walker.walkItems(item, [&](Element *) -> ElementWalker::VisitResult {
            ++counter;
            if (counter == 1)
                return ElementWalker::StopWalk;
            return ElementWalker::RecursiveWalk;
        });
        return counter;
    };

    State root;
    State s1(&root);
    State s2(&root);

    // test pre-order traversal
    QVector<Element *> seenStates = walkAllStates(ElementWalker(), &root);
    QCOMPARE(seenStates, QVector<Element *>() << &root << &s1 << &s2);
    // check if walker aborts if requested
    int count = walkOneState(ElementWalker(), &root);
    QCOMPARE(count, 1);

    // test post-order traversal
    seenStates = walkAllStates(ElementWalker(ElementWalker::PostOrderTraversal), &root);
    QCOMPARE(seenStates, QVector<Element *>() << &s1 << &s2 << &root);
    // check if walker aborts if requested
    count = walkOneState(ElementWalker(ElementWalker::PostOrderTraversal), &root);
    QCOMPARE(count, 1);
}

QTEST_MAIN(UtilTest)

#include "test_util.moc"
