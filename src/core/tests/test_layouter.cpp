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

#include <config-test.h>

#include "util.h"

#include "layouter.h"
#include "layerwiselayouter.h"
#include "layoutproperties.h"
#include "objecthelper.h"
#include "parsehelper.h"
#include "scxmlimporter.h"
#include "state.h"
#include "transition.h"

#include <QTest>
#include <QFile>
#include <QFileInfo>

#define QVERIFY_RETURN(statement, retval)                                     \
    do {                                                                      \
        if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) \
            return retval;                                                    \
    } while (0)

using namespace KDSME;
using namespace ObjectHelper;

class LayouterTest : public QObject
{
    Q_OBJECT

protected Q_SLOTS:
    void testBasicState();
    void testParallelState();

private:
    static void assertStatesHorizontallyAligned(const QList<State *> &states, qreal epsilonY = 1.0)
    {
        QVERIFY(!states.isEmpty());
        const State *reference = states[0];

        const qreal referenceCenterY = reference->pos().y() + (reference->height() / 2);
        for (State *item : states) {
            const qreal centerY = item->pos().y() + (item->height() / 2);
            QVERIFY2(qAbs(centerY - referenceCenterY) < epsilonY, "Not horizontally aligned");
        }
    }

    static void assertStatesVerticallyAligned(const QList<State *> &states, qreal epsilonX = 1.0)
    {
        QVERIFY(!states.isEmpty());
        const State *reference = states[0];

        const qreal referenceCenterX = reference->pos().x() + (reference->width() / 2);
        for (State *state : states) {
            const qreal centerX = state->pos().x() + (state->width() / 2);
            QVERIFY2(qAbs(centerX - referenceCenterX) < epsilonX, "Not vertically aligned");
        }
    }

    static void assertRegionContainsStates(State *region, const QList<State *> &states)
    {
        const QRectF rect = region->boundingRect();
        for (const State *item : states) {
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
    ScxmlImporter importer(ParseHelper::readFile(QStringLiteral(TEST_DATA_DIR "/scxml/basicstate.scxml")));
    const QScopedPointer<StateMachine> machine(importer.import());

    auto elements = machine->findChildren<Element *>();
    auto states = copy_if_type<State *>(elements);
    // auto transitions = copy_if_type<Transition*>(elements);
    QCOMPARE(states.size(), 4);
    // QCOMPARE(transitions.size(), 3);
}

void LayouterTest::testParallelState()
{
    /*
        State chart:
            P1:
                S1: (I) -> (S11) -> (S1Final)
                S2: (I) -> (S21) -> (S2Final)
    */
    ScxmlImporter importer(ParseHelper::readFile(QStringLiteral(TEST_DATA_DIR "/scxml/parallelstate.scxml")));
    const QScopedPointer<StateMachine> machine(importer.import());
    QVERIFY(machine);

    const LayoutProperties properties;
    LayerwiseLayouter layouter;
    layouter.layout(machine.data(), &properties);

    auto elements = machine->findChildren<Element *>();
    auto states = copy_if_type<State *>(elements);
    auto transitions = copy_if_type<Transition *>(elements);
    QCOMPARE(states.size(), 10);
    QCOMPARE(transitions.size(), 5);
    auto pureStateItems = filter(states, [](const State *state) { return state->isComposite(); });

    assertRegionContainsStates(machine.data(), pureStateItems);
}

QTEST_MAIN(LayouterTest)

#include "test_layouter.moc"
