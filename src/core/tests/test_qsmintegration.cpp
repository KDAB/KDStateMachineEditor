/*
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
#include "runtimecontroller.h"
#include "util.h"

#include <config-kdsme.h>

#if Qt5RemoteObjects_VERSION < QT_VERSION_CHECK(5, 9, 0)
#include "debuginterface_replica.h"
#else
#include "rep_debuginterface_replica.h"
#endif

#include "debuginterfaceclient.h"
#include "qsmdebuginterfacesource.h"

#include <QtTest>
#include <QFile>
#include <QFileInfo>
#include <QRemoteObjectNode>
#include <QString>

#define QVERIFY_RETURN(statement, retval) \
    do { if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) return retval; } while (0)

using namespace KDSME;

struct QsmAdapter : public DebugInterfaceClient
{
    QRemoteObjectRegistryHost registryNode;
    QRemoteObjectNode clientNode;
    QsmDebugInterfaceSource interface;

    QsmAdapter(QObject* parent = nullptr)
        : DebugInterfaceClient(parent)
        , registryNode(QUrl(QStringLiteral("local:registry")))
        , clientNode(QUrl(QStringLiteral("local:registry")))
    {
        // set up the debug interface on the local registry and connect to it
        // this is simpler than writing another class that handles in-process debuggging
        // just pay the cost for the in-process communication, it's not that much anyway
        registryNode.enableRemoting(interface.remoteObjectSource());

        auto interfaceReplica = clientNode.acquire<DebugInterfaceReplica>();
        interfaceReplica->waitForSource();
        setDebugInterface(interfaceReplica);
    }
};

class QsmIntegrationTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testEmptyInput();
    void testSimpleQSM();
    void testRunningQSM();
};

void QsmIntegrationTest::testEmptyInput()
{
    QsmAdapter adapter;
    QVERIFY(!adapter.machine());
}

void QsmIntegrationTest::testSimpleQSM()
{
    QStateMachine qsm;
    qsm.setObjectName("myStateMachine");

    QsmAdapter adapter;
    QSignalSpy spy(&adapter, &QsmAdapter::repopulateView);
    // initial repopulation, it results in an empty machine
    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);
    QVERIFY(!adapter.machine());

    // set the debuggee state machine, another repopulation will happen
    adapter.interface.setQStateMachine(&qsm);
    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 2);

    const StateMachine* machine = adapter.machine();
    QVERIFY(machine);
    QCOMPARE(machine->label(), QStringLiteral("myStateMachine"));
    QCOMPARE(machine->runtimeController()->isRunning(), false);
}

void QsmIntegrationTest::testRunningQSM()
{
    QStateMachine qsm;
    qsm.setObjectName("myStateMachine");
    QState qsmInitial(&qsm);
    qsmInitial.setObjectName("initial");
    qsm.setInitialState(&qsmInitial);

    QFinalState qsmFinal(&qsm);
    qsmFinal.setObjectName("final");
    qsm.addState(&qsmFinal);

    QTimer timer;
    timer.setInterval(10);
    timer.setSingleShot(true);
    qsmInitial.addTransition(&timer, SIGNAL(timeout()), &qsmFinal);

    qsm.start();

    QsmAdapter adapter;
    QSignalSpy spy(&adapter, &QsmAdapter::repopulateView);
    QVERIFY(spy.wait(20));
    adapter.interface.setQStateMachine(&qsm);
    QVERIFY(spy.wait(20));

    const StateMachine* machine = adapter.machine();
    QVERIFY(machine);
    QCOMPARE(machine->label(), QStringLiteral("myStateMachine"));
    QCOMPARE(machine->childStates().size(), 3); // pseudo state + "initial"

    const auto runtime = machine->runtimeController();

    QCOMPARE(runtime->isRunning(), true);
    QCOMPARE(runtime->activeConfiguration().size(), 1);
    QCOMPARE(runtime->activeConfiguration().values()[0]->label(), QStringLiteral("initial"));
    QCOMPARE(runtime->lastTransitions().size(), 0);

    timer.start();
    QVERIFY(TestUtil::waitForSignal(runtime, SIGNAL(isRunningChanged(bool))));

    QCOMPARE(runtime->isRunning(), false);
    QCOMPARE(runtime->activeConfiguration().size(), 1);
    QCOMPARE(runtime->activeConfiguration().values()[0]->label(), QStringLiteral("final"));
    QCOMPARE(runtime->lastTransitions().size(), 1);
}

QTEST_MAIN(QsmIntegrationTest)

#include "test_qsmintegration.moc"
