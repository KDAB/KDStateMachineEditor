/*
  test_commands.cpp

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

#include "command/createelementcommand.h"
#include "command/modifylayoutitemcommand.h"
#include "command/modifytransitioncommand.h"
#include "command/modifytransitionlayoutitemcommand.h"
#include "command/modifypropertycommand.h"
#include "command/reparentelementcommand.h"
#include "commandcontroller.h"
#include "element.h"
#include "elementmodel.h"
#include "layoutitem.h"
#include "view.h"

#include <QDebug>
#include <QAction>
#include <QtTest>

using namespace KDSME;

class TestHarness
{
public:
    TestHarness();

    StateMachine machine;
    StateModel model;
    View view;
    QUndoStack undoStack;
};

TestHarness::TestHarness()
{
    model.setState(&machine);
    view.setStateMachine(&machine);
}

class CommandsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAddState();
    void testAddTransition();
    void testLayoutSnapshot();
    void testModifyProperty();
    void testModifyTransition();
    void testModifyLayoutItem_moveBy();
    void testModifyLayoutItem_setGeometry();
    void testModifyTransitionLayoutItem();
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
    CreateElementCommand* cmd = new CreateElementCommand(&harness.model, Element::StateType);
    harness.undoStack.push(cmd);
    QCOMPARE(harness.machine.childStates().size(), 1);

    // add transition to state
    State* state = harness.machine.childStates()[0];
    cmd = new CreateElementCommand(&harness.model, Element::TransitionType);
    cmd->setParentElement(state);
    harness.undoStack.push(cmd);
    QCOMPARE(harness.machine.childStates().size(), 1);
    QCOMPARE(harness.machine.childStates()[0]->transitions().size(), 1);

    // remove transition
    harness.undoStack.undo();
    QCOMPARE(harness.machine.childStates().size(), 1); // state is still there
    QCOMPARE(harness.machine.childStates()[0]->transitions().size(), 0); // transition is gone
}

void CommandsTest::testLayoutSnapshot()
{
    StateMachine machine;
    State state(&machine);
    Transition transition(&machine);

    View view;
    view.setStateMachine(&machine);

    auto machineItem = qobject_cast<StateLayoutItem*>(view.rootLayoutItem());
    QVERIFY(machineItem);
    machineItem->setPos(QPointF(10, 10));
    machineItem->setWidth(100);
    machineItem->setHeight(100);
    auto stateItem = qobject_cast<StateLayoutItem*>(view.rootLayoutItem()->childStates().value(0));
    QVERIFY(stateItem);
    stateItem->setPos(QPointF(20, 20));
    stateItem->setWidth(200);
    stateItem->setHeight(200);
    auto transitionItem = qobject_cast<TransitionLayoutItem*>(view.rootLayoutItem()->transitions().value(0));
    QVERIFY(transitionItem);
    transitionItem->setPos(QPointF(30, 30));
    transitionItem->setShape(QPainterPath(QPointF(300, 300)));

    // snapshot current layout
    // TODO: Waiting for Bogdan's patch

    /*
    // modify
    machineItem->setPos(QPointF(11, 11));
    machineItem->setWidth(101);
    machineItem->setHeight(101);
    stateItem->setPos(QPointF(11, 11));
    stateItem->setWidth(101);
    stateItem->setHeight(101);
    transitionItem->setPos(QPointF(31, 31));
    transitionItem->setShape(QPainterPath(QPointF(301, 301)));
    */

    // restore snapshot
    // TODO: Waiting for Bogdan's patch

    QCOMPARE(machineItem->pos(), QPointF(10, 10));
    QCOMPARE(machineItem->width(), 100.);
    QCOMPARE(machineItem->height(), 100.);
    QCOMPARE(stateItem->pos(), QPointF(20, 20));
    QCOMPARE(stateItem->width(), 200.);
    QCOMPARE(stateItem->height(), 200.);
    QCOMPARE(transitionItem->pos(), QPointF(30, 30));
    QCOMPARE(transitionItem->shape(), QPainterPath(QPointF(300, 300)));
}

void CommandsTest::testModifyProperty()
{
    TestHarness harness;
    QAction action("foo", nullptr);

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
    object.insert("checkable", true);
    object.insert("autoRepeat", false);
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

void CommandsTest::testModifyTransition()
{
    TestHarness harness;

    {
        Transition transition;
        State newSourceState;
        QVERIFY(!transition.sourceState());
        auto cmd = new ModifyTransitionCommand(&transition);
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
        auto cmd = new ModifyTransitionCommand(&transition);
        cmd->setTargetState(&newTargetState);
        QVERIFY(!transition.targetState());
        harness.undoStack.push(cmd);
        QCOMPARE(transition.targetState(), &newTargetState);
        harness.undoStack.undo();
        QVERIFY(!transition.targetState());
    }
}

void CommandsTest::testModifyLayoutItem_moveBy()
{
    TestHarness harness;
    LayoutItem item;

    QCOMPARE(item.pos(), QPointF(0, 0));
    auto cmd = new ModifyLayoutItemCommand(&item);
    QCOMPARE(item.pos(), QPointF(0, 0));
    cmd->moveBy(5, 5);
    harness.undoStack.push(cmd);
    QCOMPARE(item.pos(), QPointF(5, 5));
    harness.undoStack.undo();
    QCOMPARE(item.pos(), QPointF(0, 0));
}

void CommandsTest::testModifyLayoutItem_setGeometry()
{
    TestHarness harness;
    LayoutItem item;

    QRectF newGeometry(5, 5, 10, 10);

    QCOMPARE(item.pos(), QPointF(0, 0));
    QCOMPARE(item.width(), 0.);
    QCOMPARE(item.height(), 0.);
    auto cmd = new ModifyLayoutItemCommand(&item);
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

void CommandsTest::testModifyTransitionLayoutItem()
{
    TestHarness harness;
    TransitionLayoutItem transition;

    const QPainterPath originalPath;
    const QPainterPath newPath(QPointF(5, 5));
    QCOMPARE(transition.shape(), originalPath);
    auto cmd = new ModifyTransitionLayoutItemCommand(&transition);
    QCOMPARE(transition.shape(), originalPath);
    cmd->setShape(newPath);
    harness.undoStack.push(cmd);
    QCOMPARE(transition.shape(), newPath);
    harness.undoStack.undo();
    QCOMPARE(transition.shape(), originalPath);
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
        harness.undoStack.push(cmd);
        QCOMPARE(element.parentElement(), &newParentElement);
        harness.undoStack.undo();
        QVERIFY(!element.parentElement());
    }

    {
        Element element;
        Element oldParentElement;
        Element newParentElement;

        element.setParent(&oldParentElement);

        QCOMPARE(element.parentElement(), &oldParentElement);
        auto cmd = new ReparentElementCommand(&harness.view, &element);
        cmd->setParentElement(&newParentElement);
        QCOMPARE(element.parentElement(), &oldParentElement);
        harness.undoStack.push(cmd);
        QCOMPARE(element.parentElement(), &newParentElement);
        harness.undoStack.undo();
        QCOMPARE(element.parentElement(), &oldParentElement);

        element.setParent(nullptr); // don't double-delete element
    }
}

QTEST_MAIN(CommandsTest)

#include "test_commands.moc"
