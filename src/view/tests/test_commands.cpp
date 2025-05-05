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

#include "command/createelementcommand_p.h"
#include "command/modifyelementcommand_p.h"
#include "command/modifytransitioncommand_p.h"
#include "command/modifypropertycommand_p.h"
#include "command/reparentelementcommand_p.h"
#include "commandcontroller.h"
#include "state.h"
#include "transition.h"
#include "elementmodel.h"
#include "statemachinescene.h"

#include "debug.h"
#include <QAction>
#include <QJsonObject>
#include <QTest>

using namespace KDSME;

class TestHarness
{
public:
    TestHarness();

    StateMachine machine;
    StateModel model;
    StateMachineScene view;
    QUndoStack undoStack;
};

TestHarness::TestHarness()
{
    model.setState(&machine);
    view.setRootState(&machine);
}

class CommandsTest : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void testAddState();
    void testAddTransition();
    void testLayoutSnapshot();
    void testModifyProperty();
    void testModifyTransition();
    void testModifyElement_moveBy();
    void testModifyElement_setGeometry();
    void testReparentElement();
};

void CommandsTest::testAddState()
{
    TestHarness harness;
    QCOMPARE(harness.machine.childStates().size(), 0);

    // add state
    auto cmd = new CreateElementCommand(&harness.model, Element::StateType);
    harness.undoStack.push(cmd);
    QCOMPARE(harness.machine.childStates().size(), 1);

    // remove state
    harness.undoStack.undo();
    QCOMPARE(harness.machine.childStates().size(), 0);
}

void CommandsTest::testAddTransition()
{
    TestHarness harness;
    QCOMPARE(harness.machine.childStates().size(), 0);

    // add state
    auto *cmd = new CreateElementCommand(&harness.model, Element::StateType);
    harness.undoStack.push(cmd);
    QCOMPARE(harness.machine.childStates().size(), 1);

    // add transition to state
    State *state = harness.machine.childStates().at(0);
    cmd = new CreateElementCommand(&harness.model, Element::SignalTransitionType);
    cmd->setParentElement(state);
    harness.undoStack.push(cmd);
    QCOMPARE(harness.machine.childStates().size(), 1);
    QCOMPARE(harness.machine.childStates().at(0)->transitions().size(), 1);

    // remove transition
    harness.undoStack.undo();
    QCOMPARE(harness.machine.childStates().size(), 1); // state is still there
    QCOMPARE(harness.machine.childStates().at(0)->transitions().size(), 0); // transition is gone
}

void CommandsTest::testLayoutSnapshot()
{
    StateMachine machine;
    State state(&machine);
    Transition transition(&machine);

    StateMachineScene view;
    view.setRootState(&machine);

    machine.setPos(QPointF(10, 10));
    machine.setWidth(100);
    machine.setHeight(100);
    state.setPos(QPointF(20, 20));
    state.setWidth(200);
    state.setHeight(200);
    transition.setPos(QPointF(30, 30));
    transition.setShape(QPainterPath(QPointF(300, 300)));

    // snapshot current layout
    // TODO: Waiting for Bogdan's patch

    /*
    // modify
    machine->setPos(QPointF(11, 11));
    machine->setWidth(101);
    machine->setHeight(101);
    state->setPos(QPointF(11, 11));
    state->setWidth(101);
    state->setHeight(101);
    transition->setPos(QPointF(31, 31));
    transition->setShape(QPainterPath(QPointF(301, 301)));
    */

    // restore snapshot
    // TODO: Waiting for Bogdan's patch

    QCOMPARE(machine.pos(), QPointF(10, 10));
    QCOMPARE(machine.width(), 100.);
    QCOMPARE(machine.height(), 100.);
    QCOMPARE(state.pos(), QPointF(20, 20));
    QCOMPARE(state.width(), 200.);
    QCOMPARE(state.height(), 200.);
    QCOMPARE(transition.pos(), QPointF(30, 30));
    QCOMPARE(transition.shape(), QPainterPath(QPointF(300, 300)));
}

void CommandsTest::testModifyProperty() // NOLINT(readability-function-cognitive-complexity)
{
    TestHarness harness;
    QAction action(QStringLiteral("foo"), nullptr);

    // via (const char*, QVariant) ctor
    QCOMPARE(action.isCheckable(), false);
    auto cmd = new ModifyPropertyCommand(&action, "checkable", true);
    QCOMPARE(action.isCheckable(), false);
    harness.undoStack.push(cmd);
    QCOMPARE(action.isCheckable(), true);
    harness.undoStack.undo();
    QCOMPARE(action.isCheckable(), false);

    // via QJsonObject ctor
    QCOMPARE(action.isCheckable(), false);
    QCOMPARE(action.autoRepeat(), true);
    QJsonObject object;
    object.insert(QStringLiteral("checkable"), true);
    object.insert(QStringLiteral("autoRepeat"), false);
    cmd = new ModifyPropertyCommand(&action, object);
    QCOMPARE(action.isCheckable(), false);
    QCOMPARE(action.autoRepeat(), true);
    harness.undoStack.push(cmd);
    QCOMPARE(action.isCheckable(), true);
    QCOMPARE(action.autoRepeat(), false);
    harness.undoStack.undo();
    QCOMPARE(action.isCheckable(), false);
    QCOMPARE(action.autoRepeat(), true);
}

void CommandsTest::testModifyTransition() // NOLINT(readability-function-cognitive-complexity)
{
    TestHarness harness;

    {
        Transition transition;
        State newSourceState;
        QVERIFY(!transition.sourceState());
        auto cmd = new ModifyTransitionCommand(&transition, nullptr);
        cmd->setSourceState(&newSourceState);
        QVERIFY(!transition.sourceState());
        harness.undoStack.push(cmd);
        QCOMPARE(transition.sourceState(), &newSourceState);
        harness.undoStack.undo();
        QVERIFY(!transition.sourceState());
    }

    {
        Transition transition;
        State newTargetState;
        QVERIFY(!transition.targetState());
        auto cmd = new ModifyTransitionCommand(&transition, nullptr);
        cmd->setTargetState(&newTargetState);
        QVERIFY(!transition.targetState());
        harness.undoStack.push(cmd);
        QCOMPARE(transition.targetState(), &newTargetState);
        harness.undoStack.undo();
        QVERIFY(!transition.targetState());
    }

    {
        Transition transition;
        const QPainterPath originalPath;
        const QPainterPath newPath(QPointF(5, 5));
        QCOMPARE(transition.shape(), originalPath);
        auto cmd = new ModifyTransitionCommand(&transition, nullptr);
        QCOMPARE(transition.shape(), originalPath);
        cmd->setShape(newPath);
        harness.undoStack.push(cmd);
        QCOMPARE(transition.shape(), newPath);
        harness.undoStack.undo();
        QCOMPARE(transition.shape(), originalPath);
    }
}

void CommandsTest::testModifyElement_moveBy()
{
    TestHarness harness;
    Element item;

    QCOMPARE(item.pos(), QPointF(0, 0));
    auto cmd = new ModifyElementCommand(&item);
    QCOMPARE(item.pos(), QPointF(0, 0));
    cmd->moveBy(5, 5);
    harness.undoStack.push(cmd);
    QCOMPARE(item.pos(), QPointF(5, 5));
    harness.undoStack.undo();
    QCOMPARE(item.pos(), QPointF(0, 0));
}

void CommandsTest::testModifyElement_setGeometry() // NOLINT(readability-function-cognitive-complexity)
{
    TestHarness harness;
    Element item;

    const QRectF newGeometry(5, 5, 10, 10);

    QCOMPARE(item.pos(), QPointF(0, 0));
    QCOMPARE(item.width(), 0.);
    QCOMPARE(item.height(), 0.);
    auto cmd = new ModifyElementCommand(&item);
    QCOMPARE(item.pos(), QPointF(0, 0));
    QCOMPARE(item.width(), 0.);
    QCOMPARE(item.height(), 0.);
    cmd->setGeometry(newGeometry);
    harness.undoStack.push(cmd);
    QCOMPARE(item.pos(), QPointF(5, 5));
    QCOMPARE(item.width(), 10.);
    QCOMPARE(item.height(), 10.);
    harness.undoStack.undo();
    QCOMPARE(item.pos(), QPointF(0, 0));
    QCOMPARE(item.width(), 0.);
    QCOMPARE(item.height(), 0.);
}

void CommandsTest::testReparentElement()
{
    TestHarness harness;

    {
        Element element;
        Element newParentElement;

        QVERIFY(!element.parentElement());
        auto cmd = new ReparentElementCommand(&harness.view, &element);
        cmd->setParentElement(&newParentElement);
        QVERIFY(!element.parentElement());
        harness.undoStack.push(cmd); // should be a no-op
        QVERIFY(!element.parentElement());
        harness.undoStack.undo();
        QVERIFY(!element.parentElement());
    }

    {
        Element element(&harness.machine);
        Element newParentElement(&harness.machine);

        QCOMPARE(element.parentElement(), &harness.machine);
        auto cmd = new ReparentElementCommand(&harness.view, &element);
        cmd->setParentElement(&newParentElement);
        QCOMPARE(element.parentElement(), &harness.machine);
        harness.undoStack.push(cmd);
        QCOMPARE(element.parentElement(), &newParentElement);
        harness.undoStack.undo();
        QCOMPARE(element.parentElement(), &harness.machine);

        element.setParent(nullptr); // don't double-delete element
    }
}

QTEST_MAIN(CommandsTest)

#include "test_commands.moc"
