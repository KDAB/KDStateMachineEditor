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

#include "export/scxmlexporter.h"
#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QTest>

#define QVERIFY_RETURN(statement, retval)                                     \
    do {                                                                      \
        if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) \
            return retval;                                                    \
    } while (0)

using namespace KDSME;

class ScxmlExportTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testEmptyInput();
    void testEmptyLabels();

    void testSimpleStateMachine();
};

void ScxmlExportTest::testEmptyInput()
{
    QByteArray output;
    ScxmlExporter exporter(&output);
    const bool success = exporter.exportMachine(nullptr);
    QVERIFY(!success);
    QVERIFY(output.isEmpty());
}

void ScxmlExportTest::testEmptyLabels()
{
    StateMachine root;
    const State s1(&root);
    const State s2(&root);

    QByteArray output;
    ScxmlExporter exporter(&output);
    const bool success = exporter.exportMachine(&root);
    QVERIFY(!success);
    QVERIFY(exporter.errorString().contains(u"empty label"));
}

void ScxmlExportTest::testSimpleStateMachine()
{
    StateMachine root;
    root.setLabel(QStringLiteral("root"));
    State s1(&root);
    s1.setLabel(QStringLiteral("s1"));
    State s2(&root);
    s2.setLabel(QStringLiteral("s2"));
    Transition *t1 = s1.addSignalTransition(&s2);
    t1->setLabel(QStringLiteral("e1"));
    PseudoState initialState(PseudoState::InitialState, &root);
    initialState.addSignalTransition(&s1);

    QByteArray output;
    ScxmlExporter exporter(&output);
    const bool success = exporter.exportMachine(&root);
    QVERIFY(success);
    QVERIFY(!output.isEmpty());
}

QTEST_MAIN(ScxmlExportTest)

#include "test_scxmlexport.moc"
