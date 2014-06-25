/*
  test_layouter.cpp

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

#include <config-test.h>

#include "util.h"

#include "layouter.h"
#include "layoutitem.h"
#include "layerwiselayouter.h"
#include "objecthelper.h"
#include "parsehelper.h"
#include "scxmlparser.h"
#include "element.h"
#include "view/view.h"

#include <QtTest>
#include <QFile>
#include <QFileInfo>

#define QVERIFY_RETURN(statement, retval) \
    do { if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) return retval; } while (0)

using namespace ObjectHelper;
using namespace KDSME;

class LayouterTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testBasicState();
    void testParallelState();

private:
    static void assertStatesHorizontallyAligned(const QList<StateLayoutItem*>& items, qreal epsilonY = 1.0)
    {
        QVERIFY(!items.isEmpty());
        const StateLayoutItem* reference = items[0];

        const qreal referenceCenterY = reference->pos().y() + reference->height()/2;
        foreach (StateLayoutItem* item, items) {
            const qreal centerY = item->pos().y() + item->height()/2;
            QVERIFY2(qAbs(centerY - referenceCenterY) < epsilonY, "Not horizontally aligned");
        }
    }

    static void assertStatesVerticallyAligned(const QList<StateLayoutItem*>& items, qreal epsilonX = 1.0)
    {
        QVERIFY(!items.isEmpty());
        const StateLayoutItem* reference = items[0];

        const qreal referenceCenterX = reference->pos().x() + reference->width()/2;
        foreach (StateLayoutItem* item, items) {
            const qreal centerX = item->pos().x() + item->width()/2;
            QVERIFY2(qAbs(centerX - referenceCenterX) < epsilonX, "Not vertically aligned");
        }
    }

    static void assertRegionContainsStates(StateLayoutItem* region, const QList<StateLayoutItem*> items)
    {
        const QRectF rect = region->boundingRect();
        foreach (StateLayoutItem* item, items) {
            QVERIFY(rect.contains(item->boundingRect()));
        }
    }
};

void LayouterTest::testBasicState()
{
    /*
        State chart:
            (I) -> (S1) -> (S2) -> (Final)
    */
    QScopedPointer<StateMachine> machine(ParseHelper::parseFile("scxml/basicstate.scxml"));

    View view;
    view.setStateMachine(machine.data());
    view.layout();

    QList<LayoutItem*> layoutItems = view.layoutItems();
    auto stateItems = copy_if_type<StateLayoutItem*>(layoutItems);
    auto transitionItems = copy_if_type<TransitionLayoutItem*>(layoutItems);
    QCOMPARE(stateItems.size(), 5);
    //QCOMPARE(transitionItems.size(), 3);


    StateLayoutItem* rootStateItem = qobject_cast<StateLayoutItem*>(view.rootLayoutItem());
    QVERIFY(rootStateItem);
    QCOMPARE(rootStateItem->element(), machine.data());
    auto pureStateItems = filter(stateItems, [](const StateLayoutItem* item) { return qobject_cast<State*>(item->element())->isComposite(); });

    assertStatesVerticallyAligned(pureStateItems);
    assertRegionContainsStates(rootStateItem, pureStateItems);
}

void LayouterTest::testParallelState()
{
    /*
        State chart:
            P1:
                S1: (I) -> (S11) -> (S1Final)
                S2: (I) -> (S21) -> (S2Final)
    */
    QScopedPointer<StateMachine> machine(ParseHelper::parseFile("scxml/parallelstate.scxml"));
    QVERIFY(machine);

    View view;
    view.setStateMachine(machine.data());
    view.layout();

    QList<LayoutItem*> layoutItems = view.layoutItems();
    auto stateItems = copy_if_type<StateLayoutItem*>(layoutItems);
    auto transitionItems = copy_if_type<TransitionLayoutItem*>(layoutItems);
    QCOMPARE(stateItems.size(), 11);
    QCOMPARE(transitionItems.size(), 5);

    StateLayoutItem* rootStateItem = qobject_cast<StateLayoutItem*>(view.rootLayoutItem());
    QVERIFY(rootStateItem);
    QCOMPARE(rootStateItem->element(), machine.data());
    auto pureStateItems = filter(stateItems, [](const StateLayoutItem* item) { return qobject_cast<State*>(item->element())->isComposite(); });

    assertRegionContainsStates(rootStateItem, pureStateItems);
}

QTEST_MAIN(LayouterTest)

#include "test_layouter.moc"
