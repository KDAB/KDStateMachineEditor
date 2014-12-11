/*
  test_scxmlexport.cpp

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

#include "export/scxmlexporter.h"
#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QtTest>

#ifdef QT_XMLPATTERNS_LIB
#include <QXmlQuery>
#endif

#define QVERIFY_RETURN(statement, retval) \
    do { if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) return retval; } while (0)

using namespace KDSME;

namespace {

#ifdef QT_XMLPATTERNS_LIB
QStringList query(QByteArray xml, const QString& sourceCode)
{
    static const QString preamble = \
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
    Transition* t1 = s1.addTransition(&s2);
    t1->setLabel("e1");
    PseudoState initialState(PseudoState::InitialState, &root);
    initialState.addTransition(&s1);

    QByteArray output;
    ScxmlExporter exporter(&output);
    bool success = exporter.exportMachine(&root);
    QVERIFY(success);
    QVERIFY(!output.isEmpty());

#ifdef QT_XMLPATTERNS_LIB
    QStringList stateMachineIds = query(output, "doc($input)/scxml/@name/string()");
    QCOMPARE(stateMachineIds, QStringList() << "root");
    QStringList stateIds = query(output, "doc($input)/scxml/state/@id/string()");
    QCOMPARE(stateIds, QStringList() << "s1" << "s2");
    QStringList initialStateIds = query(output, "doc($input)/scxml/@initial/string()");
    QCOMPARE(initialStateIds, QStringList() << "s1");
    QStringList transitions = query(output, "doc($input)/scxml/state[@id='s1']/transition/@event/string()");
    QCOMPARE(transitions, QStringList() << "e1");
#endif
}

QTEST_MAIN(ScxmlExportTest)

#include "test_scxmlexport.moc"
