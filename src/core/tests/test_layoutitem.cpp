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

#include "element.h"
#include "layout/layoututils.h"

#include <QTest>

using namespace KDSME;

class ElementTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAbsolutePos();

    void testLayoutUtils_moveToParent();
};

void ElementTest::testAbsolutePos()
{
    Element i1;
    const QPointF i1Pos = QPointF(5, 5);
    i1.setPos(i1Pos);
    QCOMPARE(i1.pos(), i1Pos);
    QCOMPARE(i1.absolutePos(), i1Pos);

    Element parent;
    parent.setPos(QPointF(10, 10));
    i1.setParent(&parent);
    QCOMPARE(i1.pos(), QPointF(5, 5));
    QCOMPARE(i1.absolutePos(), QPointF(15, 15));

    i1.setParent(nullptr);
}

void ElementTest::testLayoutUtils_moveToParent()
{
    Element i1;
    const QPointF i1Pos = QPointF(5, 5);
    i1.setPos(i1Pos);
    QCOMPARE(i1.pos(), i1Pos);
    QCOMPARE(i1.absolutePos(), i1Pos);

    Element parent;
    parent.setPos(QPointF(10, 10));
    LayoutUtils::moveToParent(&i1, &parent);
    QCOMPARE(i1.pos(), QPointF(-5, -5));
    // item should stay at its initial position
    QCOMPARE(i1.absolutePos(), QPointF(5, 5));

    i1.setParent(nullptr);
}

QTEST_MAIN(ElementTest)

#include "test_layoutitem.moc"
