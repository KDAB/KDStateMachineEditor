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

#include "export/qmlexporter.h"
#include "state.h"
#include "transition.h"

#include <QTest>
#include <QFile>
#include <QFileInfo>

#define QVERIFY_RETURN(statement, retval) \
    do { if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)) return retval; } while (0)

using namespace KDSME;

namespace {
    static QString stripQml(const QByteArray &input)
    {
        QStringList result;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        Q_FOREACH(const QString &s, QString::fromUtf8(input).split('\n', Qt::SkipEmptyParts)) {
#else
        Q_FOREACH(const QString &s, QString::fromUtf8(input).split('\n', QString::SkipEmptyParts)) {
#endif
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
    /*
    const QByteArray expectedOutput = "import QtQml.StateMachine 1.0\n\n"
        "StateMachine {\n"
        "id: root\n"
        "State {\n"
        "id: s1_foo\n"
        "}\n"
        "}\n";
    */
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
        "State {\n"
        "id: s1\n"
        "SignalTransition {\n"
        "id: e1\n"
        "targetState: s2\n"
        "}\n"
        "}\n"
        "State {\n"
        "id: s2\n"
        "TimeoutTransition {\n"
        "id: e2\n"
        "targetState: s1\n"
        "timeout: 1000\n"
        "}\n"
        "}\n"
        "}\n";

    StateMachine root;
    root.setLabel("root");
    State s1(&root);
    s1.setLabel("s1");
    State s2(&root);
    s2.setLabel("s2");
    Transition* t1 = s1.addSignalTransition(&s2);
    t1->setLabel("e1");
    Transition* t2 = s2.addTimeoutTransition(&s1, 1000);
    t2->setLabel("e2");
    PseudoState initialState(PseudoState::InitialState, &root);
    initialState.addSignalTransition(&s1);

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
