/*
  test_statemachine.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QtTest>

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
    machine.setLabel("machine");
    State s1(&machine);
    s1.setLabel("s1");

    QCOMPARE(machine.label(), QLatin1String("machine"));
    QCOMPARE(s1.label(), QLatin1String("s1"));
}

void StateMachineTest::testParentChildRelationship()
{
    StateMachine machine;
    State s1(&machine);
    Transition t1(&s1);
    State s11(&s1);
    Transition t11(&s11);

    QCOMPARE(machine.childStates(), QList<State*>() << &s1);
    QVERIFY(machine.transitions().isEmpty());

    QCOMPARE(s1.machine(), &machine);
    QCOMPARE(s1.childStates(),  QList<State*>() << &s11);
    QCOMPARE(s1.transitions(), QList<Transition*>() << &t1);
    QCOMPARE(t1.sourceState(),  &s1);

    QCOMPARE(s11.machine(), &machine);
    QVERIFY(s11.childStates().isEmpty());
    QCOMPARE(s11.transitions(), QList<Transition*>() << &t11);
    QCOMPARE(t11.sourceState(), &s11);
}

QTEST_MAIN(StateMachineTest)

#include "test_statemachine.moc"
