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

#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QTest>

using namespace KDSME;

class StateMachineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testProperties();
    void testParentChildRelationship();
};

void StateMachineTest::testProperties()
{
    State machine;
    machine.setLabel(QStringLiteral("machine"));
    State s1(&machine);
    s1.setLabel(QStringLiteral("s1"));

    QCOMPARE(machine.label(), QLatin1String("machine"));
    QCOMPARE(s1.label(), QLatin1String("s1"));
}

void StateMachineTest::testParentChildRelationship() // NOLINT(readability-function-cognitive-complexity)
{
    StateMachine machine;
    State s1(&machine);
    Transition t1(&s1);
    State s11(&s1);
    Transition t11(&s11);

    QCOMPARE(machine.childStates(), QList<State *>() << &s1);
    QVERIFY(machine.transitions().isEmpty());

    QCOMPARE(s1.machine(), &machine);
    QCOMPARE(s1.childStates(), QList<State *>() << &s11);
    QCOMPARE(s1.transitions(), QList<Transition *>() << &t1);
    QCOMPARE(t1.sourceState(), &s1);

    QCOMPARE(s11.machine(), &machine);
    QVERIFY(s11.childStates().isEmpty());
    QCOMPARE(s11.transitions(), QList<Transition *>() << &t11);
    QCOMPARE(t11.sourceState(), &s11);
}

QTEST_MAIN(StateMachineTest)

#include "test_statemachine.moc"
