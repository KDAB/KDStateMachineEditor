/*
  test_util.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  This file may be distributed and/or modified under the terms of the
  GNU Lesser General Public License version 2.1 as published by the
  Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.

  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Contact info@kdab.com if any conditions of this licensing are not
  clear to you.
*/

#include "element.h"
#include "elementwalker.h"

#include <QDebug>
#include <QVector>
#include <QtTest>

using namespace KDSME;

class UtilTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testElementWalker();
};

void UtilTest::testElementWalker()
{
    /// @return List of walked states
    auto walkAllStates = [](ElementWalker&& walker, Element* item) -> QVector<Element*> {
        QVector<Element*> walkedStates;
        walker.walkItems(item, [&](Element* state) -> ElementWalker::VisitResult {
            walkedStates << state;
            return ElementWalker::RecursiveWalk;
        });
        return walkedStates;
    };

    /// @return Number of calls to our visit-lambda
    auto walkOneState = [](ElementWalker&& walker, Element* item) -> int {
        int counter = -1;
        walker.walkItems(item, [&](Element*) -> ElementWalker::VisitResult {
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
    QVector<Element*> seenStates = walkAllStates(ElementWalker(), &root);
    QCOMPARE(seenStates, QVector<Element*>() << &root << &s1 << &s2);
    // check if walker aborts if requested
    int count = walkOneState(ElementWalker(), &root);
    QCOMPARE(count, 1);

    // test post-order traversal
    seenStates = walkAllStates(ElementWalker(ElementWalker::PostOrderTraversal), &root);
    QCOMPARE(seenStates, QVector<Element*>() << &s1 << &s2 << &root);
    // check if walker aborts if requested
    count = walkOneState(ElementWalker(ElementWalker::PostOrderTraversal), &root);
    QCOMPARE(count, 1);
}

QTEST_MAIN(UtilTest)

#include "test_util.moc"
