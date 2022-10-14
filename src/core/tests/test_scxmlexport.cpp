/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#ifdef USE_QT_XMLPATTERNS_LIB
#include <QBuffer>
#include <QXmlQuery>
#endif

#define QVERIFY_RETURN(statement, retval)                                     \
    do {                                                                      \
        if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) \
            return retval;                                                    \
    } while (0)

using namespace KDSME;

namespace {

#ifdef USE_QT_XMLPATTERNS_LIB
QStringList query(QByteArray xml, const QString &sourceCode)
{
    static const QString preamble =
        "declare default element namespace \"http://www.w3.org/2005/07/scxml\";";

    QBuffer buffer(&xml);
    buffer.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("input", &buffer);
    query.setQuery(preamble + sourceCode);
    QStringList result;
    query.evaluateTo(&result);
    return result;
}
#endif

}

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
    bool success = exporter.exportMachine(nullptr);
    QVERIFY(!success);
    QVERIFY(output.isEmpty());
}

void ScxmlExportTest::testEmptyLabels()
{
    StateMachine root;
    State s1(&root);
    State s2(&root);

    QByteArray output;
    ScxmlExporter exporter(&output);
    bool success = exporter.exportMachine(&root);
    QVERIFY(!success);
    QVERIFY(exporter.errorString().contains("empty label"));
}

void ScxmlExportTest::testSimpleStateMachine()
{
    StateMachine root;
    root.setLabel("root");
    State s1(&root);
    s1.setLabel("s1");
    State s2(&root);
    s2.setLabel("s2");
    Transition *t1 = s1.addSignalTransition(&s2);
    t1->setLabel("e1");
    PseudoState initialState(PseudoState::InitialState, &root);
    initialState.addSignalTransition(&s1);

    QByteArray output;
    ScxmlExporter exporter(&output);
    bool success = exporter.exportMachine(&root);
    QVERIFY(success);
    QVERIFY(!output.isEmpty());

#ifdef USE_QT_XMLPATTERNS_LIB
    QStringList stateMachineIds = query(output, "doc($input)/scxml/@name/string()");
    QCOMPARE(stateMachineIds, QStringList() << "root");
    QStringList stateIds = query(output, "doc($input)/scxml/state/@id/string()");
    QCOMPARE(stateIds, QStringList() << "s1"
                                     << "s2");
    QStringList initialStateIds = query(output, "doc($input)/scxml/@initial/string()");
    QCOMPARE(initialStateIds, QStringList() << "s1");
    QStringList transitions = query(output, "doc($input)/scxml/state[@id='s1']/transition/@event/string()");
    QCOMPARE(transitions, QStringList() << "e1");
#endif
}

QTEST_MAIN(ScxmlExportTest)

#include "test_scxmlexport.moc"
