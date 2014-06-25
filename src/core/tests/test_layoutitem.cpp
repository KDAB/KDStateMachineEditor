/*
  test_layoutitem.cpp

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

#include "layoutitem.h"
#include "layout/layoututils.h"

#include <QtTest>

using namespace KDSME;

class LayoutItemTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAbsolutePos();

    void testLayoutUtils_moveToParent();
};

void LayoutItemTest::testAbsolutePos()
{
    LayoutItem i1;
    const QPointF i1Pos = QPointF(5, 5);
    i1.setPos(i1Pos);
    QCOMPARE(i1.pos(), i1Pos);
    QCOMPARE(i1.absolutePos(), i1Pos);

    LayoutItem parent;
    parent.setPos(QPointF(10, 10));
    i1.setParent(&parent);
    QCOMPARE(i1.pos(), QPointF(5, 5));
    QCOMPARE(i1.absolutePos(), QPointF(15, 15));

    i1.setParent(nullptr);
}

void LayoutItemTest::testLayoutUtils_moveToParent()
{
    LayoutItem i1;
    const QPointF i1Pos = QPointF(5, 5);
    i1.setPos(i1Pos);
    QCOMPARE(i1.pos(), i1Pos);
    QCOMPARE(i1.absolutePos(), i1Pos);

    LayoutItem parent;
    parent.setPos(QPointF(10, 10));
    LayoutUtils::moveToParent(&i1, &parent);
    QCOMPARE(i1.pos(), QPointF(-5, -5));
    // item should stay at its initial position
    QCOMPARE(i1.absolutePos(), QPointF(5, 5));

    i1.setParent(nullptr);
}

QTEST_MAIN(LayoutItemTest)

#include "test_layoutitem.moc"
