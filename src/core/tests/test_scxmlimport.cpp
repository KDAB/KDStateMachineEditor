/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-test.h>

#include "scxmlimporter.h"
#include "parsehelper.h"
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

namespace {

QByteArray wrapScxml(const QByteArray &content, const QByteArray &initialState = QByteArray("s"))
{
    QByteArray result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                        "<scxml version=\"1.0\" xmlns=\"http://www.w3.org/2005/07/scxml\" initial=\""
        + initialState + "\">"
        + content + "</scxml>";
    return result;
}

StateMachine *parse(const QByteArray &data)
{
    // parse
    ScxmlImporter parser(data);
    StateMachine *stateMachine = parser.import();
    if (!stateMachine) {
        qWarning() << parser.errorString();
    }
    return stateMachine;
}

StateMachine *parseFile(const QString &fileName)
{
    const QByteArray data = ParseHelper::readFile(QStringLiteral(TEST_DATA_DIR) + u'/' + fileName);
    return parse(data);
}

}

class ScxmlImportTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testEmptyInput();
    void testInvalidInput();
    void testInvalidTargetState();

    void testParseState();
    void testParseTransition();

    void testBasicState();
    void testParallelState();

    void testExampleCalculator();
    void testExampleMicrowave();
    void testExampleTrafficLight();
    void testExampleTrafficReport();
};

void ScxmlImportTest::testEmptyInput()
{
    const QByteArray data = "";

    ScxmlImporter parser(data);
    const QScopedPointer<StateMachine> stateMachine(parser.import());
    QVERIFY(!stateMachine);
    // const QString errorString = parser.errorString();
    QVERIFY(!parser.errorString().isEmpty());
}

void ScxmlImportTest::testInvalidInput()
{
    const QByteArray data = "some garbage";

    ScxmlImporter parser(data);
    const QScopedPointer<StateMachine> stateMachine(parser.import());
    QVERIFY(!stateMachine);
    QVERIFY(!parser.errorString().isEmpty());
}

void ScxmlImportTest::testInvalidTargetState()
{
    const QByteArray data = wrapScxml(
        "<state id=\"s\"><transition target=\"foo\"/></state>");

    ScxmlImporter parser(data);
    const QScopedPointer<StateMachine> stateMachine(parser.import());
    QVERIFY(!stateMachine);
    QVERIFY(!parser.errorString().isEmpty());
}

void ScxmlImportTest::testParseState() // NOLINT(readability-function-cognitive-complexity
{
    const QByteArray data = wrapScxml(
        "<state id=\"s\" initial=\"s1\">"
        "<state id=\"s1\"/>"
        "<transition event=\"e1\" target=\"s\"/>"
        "</state>");

    ScxmlImporter parser(data);
    const QScopedPointer<StateMachine> machine(parser.import());
    QVERIFY(machine);

    // check contents of <scxml>
    State *scxmlInitial = machine->childStates().at(0);
    QVERIFY(scxmlInitial);
    QCOMPARE(scxmlInitial->type(), Element::PseudoStateType);
    State *s = machine->childStates().at(1);
    QVERIFY(s);
    QCOMPARE(s->label(), QLatin1String("s"));
    QCOMPARE(s->transitions().size(), 1);

    // check contents out outer <state>
    State *sInitial = s->childStates().at(0);
    QVERIFY(sInitial);
    QCOMPARE(sInitial->type(), Element::PseudoStateType);
    State *s1 = s->childStates().at(1);
    QVERIFY(s1);
    QCOMPARE(s1->label(), QLatin1String("s1"));
}

void ScxmlImportTest::testParseTransition() // NOLINT(readability-function-cognitive-complexity
{
    const QByteArray data = wrapScxml(
        "<state id=\"s\">"
        "<transition event=\"e1\" target=\"fin\"/>"
        "</state>"
        "<final id=\"fin\"/>");

    ScxmlImporter parser(data);
    const QScopedPointer<StateMachine> machine(parser.import());
    QVERIFY(machine);

    QCOMPARE(machine->transitions().size(), 0);
    State *s = machine->childStates().at(1);
    QCOMPARE(s->label(), QLatin1String("s"));
    State *fin = machine->childStates().at(2);
    QCOMPARE(fin->label(), QLatin1String("fin"));

    QVERIFY(s);
    Transition *t1 = s->transitions().at(0);
    QCOMPARE(t1->label(), QLatin1String("e1"));
    QCOMPARE(t1->parent(), s);
    QCOMPARE(t1->sourceState(), s);
    QCOMPARE(t1->targetState(), fin);
}

void ScxmlImportTest::testBasicState()
{
    /*
        State chart:
            (I) -> (S1) -> (S2) -> (Final)
    */
    State *root = parseFile(QStringLiteral("scxml/basicstate.scxml"));
    QVERIFY(root);
    QCOMPARE(root->label(), QLatin1String("basicstate"));
    QCOMPARE(root->childStates().size(), 4);

    State *s = nullptr;
    s = root->childStates().at(0);
    auto pseudoState = qobject_cast<PseudoState *>(s);
    QCOMPARE(pseudoState->kind(), PseudoState::InitialState);
    s = root->childStates().at(1);
    QCOMPARE(s->label(), QLatin1String("S1"));
    s = root->childStates().at(2);
    QCOMPARE(s->label(), QLatin1String("S2"));
    s = root->childStates().at(3);
    QCOMPARE(s->label(), QLatin1String("Final"));
}

void ScxmlImportTest::testParallelState() // NOLINT(readability-function-cognitive-complexity
{
    /*
        State chart:
            P1:
                S1: (I) -> (S11) -> (S1Final)
                S2: (I) -> (S21) -> (S2Final)

        Special case: Contains <parallel>
    */
    State *root = parseFile(QStringLiteral("scxml/parallelstate.scxml"));
    QVERIFY(root);
    QCOMPARE(root->label(), QLatin1String("parallelstate"));
    QCOMPARE(root->childStates().size(), 2);

    // check <parallel> element
    State *s = nullptr;
    State *p1 = root->childStates().at(1);
    QCOMPARE(p1->label(), QLatin1String("P1"));
    QCOMPARE(p1->childStates().size(), 2);

    // check first region
    State *s1 = p1->childStates().at(0);
    QCOMPARE(s1->childStates().size(), 3);
    s = s1->childStates().at(0);
    auto pseudoState = qobject_cast<PseudoState *>(s);
    QVERIFY(pseudoState);
    QCOMPARE(pseudoState->kind(), PseudoState::InitialState);
    s = s1->childStates().at(1);
    QCOMPARE(s->label(), QLatin1String("S11"));
    s = s1->childStates().at(2);
    QCOMPARE(s->label(), QLatin1String("S1Final"));

    // check second region
    State *s2 = p1->childStates().at(1);
    QCOMPARE(s2->childStates().size(), 3);
    s = s1->childStates().at(0);
    pseudoState = qobject_cast<PseudoState *>(s);
    QVERIFY(pseudoState);
    QCOMPARE(pseudoState->kind(), PseudoState::InitialState);
    s = s2->childStates().at(1);
    QCOMPARE(s->label(), QLatin1String("S21"));
    s = s2->childStates().at(2);
    QCOMPARE(s->label(), QLatin1String("S2Final"));
}

void ScxmlImportTest::testExampleCalculator() // NOLINT(readability-function-cognitive-complexity
{
    /*
        State chart (transitions omitted):
            (I)
            (wrapper)
                (I)
                (on)
                    (I)
                    (ready)
                        (begin)
                        (result)
                    (negated1)
                    (operand1)
                        (...)
                    (operand1)
                        (...)
                    (opEntered)
                    (negated2)

        Special case: Contains <datamodel>, <onentry>, <onexit>
     */
    State *root = parseFile(QStringLiteral("scxml/calculator.scxml"));
    QVERIFY(root);
    QCOMPARE(root->label(), QLatin1String("Calculator"));
    QCOMPARE(root->childStates().size(), 2);

    State *wrapper = root->childStates().at(1);
    QVERIFY(wrapper);
    QCOMPARE(wrapper->label(), QLatin1String("wrapper"));
    QCOMPARE(wrapper->childStates().size(), 2);

    State *on = wrapper->childStates().at(1);
    QVERIFY(on);
    QCOMPARE(on->label(), QLatin1String("on"));
    QCOMPARE(on->childStates().size(), 7);

    //  Further tests omitted
}

void ScxmlImportTest::testExampleMicrowave() // NOLINT(readability-function-cognitive-complexity
{
    /*
        State chart (transitions omitted):
            (I)
            (off)
            (on)
                (I)
                (idle)
                (cooking)

        Special case: Contains <initial> element
     */
    State *root = parseFile(QStringLiteral("scxml/microwave.scxml"));
    QVERIFY(root);
    QCOMPARE(root->label(), QLatin1String(""));
    QCOMPARE(root->childStates().size(), 3);

    State *off = root->childStates().at(1);
    QVERIFY(off);
    QCOMPARE(off->label(), QLatin1String("off"));
    QCOMPARE(off->childStates().size(), 0);

    State *on = root->childStates().at(2);
    QVERIFY(on);
    QCOMPARE(on->label(), QLatin1String("on"));
    QCOMPARE(on->childStates().size(), 3);

    QCOMPARE(on->childStates().at(1)->label(), QLatin1String("idle"));
    QCOMPARE(on->childStates().at(2)->label(), QLatin1String("cooking"));
}

void ScxmlImportTest::testExampleTrafficLight() // NOLINT(readability-function-cognitive-complexity)
{
    /*
        State chart:
            (I) -> (redGoingYellow) -> (yellowGoingGreen) -> (greenGoingYellow) -> (yellowGoingRed) --.
                           ^--------------------------------------------------------------------------'
    */
    State *root = parseFile(QStringLiteral("scxml/trafficlight_alternative.scxml"));
    QVERIFY(root);
    QCOMPARE(root->label(), QLatin1String("trafficlight"));
    QCOMPARE(root->childStates().size(), 5);

    // check first state
    State *s1 = root->childStates().at(1);
    QCOMPARE(s1->label(), QLatin1String("redGoingYellow"));
    State *s2 = root->childStates().at(2);
    QCOMPARE(s2->label(), QLatin1String("yellowGoingGreen"));
    QCOMPARE(s1->transitions().size(), 1);
    QCOMPARE(s1->transitions().at(0)->sourceState(), s1);
    QCOMPARE(s1->transitions().at(0)->targetState(), s2);

    // check last state
    State *s4 = root->childStates().at(4);
    QCOMPARE(s4->label(), QLatin1String("yellowGoingRed"));
    QCOMPARE(s4->transitions().size(), 1);
    QCOMPARE(s4->transitions().at(0)->sourceState(), s4);
}

void ScxmlImportTest::testExampleTrafficReport()
{
    /*
        State chart:
            (I) -> (Intro)
            (PlayAds)
            (...)

        Special case: Contains <invoke>
    */
    State *root = parseFile(QStringLiteral("scxml/trafficreport.scxml"));
    QVERIFY(root);
    QCOMPARE(root->label(), QLatin1String(""));
    QCOMPARE(root->childStates().size(), 8);

    State *s = nullptr;
    s = root->childStates().at(1);
    QCOMPARE(s->label(), QLatin1String("Intro"));
    QCOMPARE(s->transitions().size(), 3);
    s = root->childStates().at(2);
    QCOMPARE(s->label(), QLatin1String("PlayAds"));
    QCOMPARE(s->transitions().size(), 2);

    // Further tests omitted
}

QTEST_MAIN(ScxmlImportTest)

#include "test_scxmlimport.moc"
