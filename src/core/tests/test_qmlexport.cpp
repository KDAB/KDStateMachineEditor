/*
  test_qmlexport.cpp

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

#include "export/qmlexporter.h"
#include "element.h"

#include <QtTest>
#include <QFile>
#include <QFileInfo>

#define QVERIFY_RETURN(statement, retval) \
    do { if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) return retval; } while (0)

using namespace KDSME;

namespace {
    static QString stripQml(const QByteArray &input)
    {
        QStringList result;
        Q_FOREACH(const QString &s, QString::fromUtf8(input).split('\n', QString::SkipEmptyParts)) {
            result.append(s.simplified().trimmed());
        }
        return result.join("\n");
    }
}

class QmlExportTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testEmptyInput();
    void testInvalidIds();

    void testSimpleStateMachine();
};

void QmlExportTest::testEmptyInput()
{
    QByteArray output;
    QmlExporter exporter(&output);
    bool success = exporter.exportMachine(nullptr);
    QVERIFY(!success);
    QVERIFY(output.isEmpty());
}

void QmlExportTest::testInvalidIds()
{
    const QByteArray expectedOutput = "import QtQml.StateMachine 1.0\n\n"
        "StateMachine {\n"
        "id: root\n"
        "StateBase {\n"
        "id: s1_foo\n"
        "}\n"
        "}\n";

    StateMachine root;
    root.setLabel("root");
    State s1(&root);
    s1.setLabel("s1.foo");

    QByteArray output;
    QmlExporter exporter(&output);
    bool success = exporter.exportMachine(&root);
    QVERIFY(success);
}

void QmlExportTest::testSimpleStateMachine()
{
    const QByteArray expectedOutput = "import QtQml.StateMachine 1.0\n\n"
        "StateMachine {\n"
        "id: root\n"
        "initialState: s1\n"
        "StateBase {\n"
        "id: s1\n"
        "Transition {\n"
        "id: e1\n"
        "targetState: s2\n"
        "}\n"
        "}\n"
        "StateBase {\n"
        "id: s2\n"
        "}\n"
        "}\n";

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
    QmlExporter exporter(&output);
    bool success = exporter.exportMachine(&root);
    QVERIFY(success);
    QVERIFY(!output.isEmpty());

    const QString output2 = stripQml(output);
    const QString expectedOutput2 = stripQml(expectedOutput);
    QCOMPARE(output2, expectedOutput2);
}


QTEST_MAIN(QmlExportTest)

#include "test_qmlexport.moc"
