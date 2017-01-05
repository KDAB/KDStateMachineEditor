/*
  test_layoutinformation.cpp

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

#include "layoutimportexport.h"
#include "layoututils.h"
#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QtTest>

using namespace KDSME;

namespace {

void compare(const Transition *state1, const Transition *state2)
{
    QCOMPARE(state1->pos(), state2->pos());
    QCOMPARE(state1->labelBoundingRect(), state2->labelBoundingRect());
    QCOMPARE(state1->shape(), state2->shape());
}

void compare(const State *state1, const State *state2)
{
    QCOMPARE(state1->label(), state2->label());
    QCOMPARE(state1->pos(), state2->pos());
    QCOMPARE(state1->width(), state2->width());
    QCOMPARE(state1->height(), state2->height());

    auto copyChildren = state2->childStates();
    foreach (const State *child, state1->childStates()) {
        auto copyChild = std::find_if(copyChildren.begin(), copyChildren.end(), [&child](State *state)->bool{return state->label() == child->label();});
        if (copyChild != copyChildren.end())
            compare(child, *copyChild);
    }

    auto copyTransitions = state2->transitions();
    foreach (const Transition *item, state1->transitions()) {
        auto copyTransition = std::find_if(copyTransitions.begin(), copyTransitions.end(), [&item](Transition *state)->bool{return state->label() == item->label();});
        if (copyTransition != copyTransitions.end()) {
            compare(item, *copyTransition);
        }
    }
}

QPainterPath createPath(const QPointF &start)
{
    QPainterPath path(start);
    for (int i = 0; i < 3; ++i ) {
        switch (i) {
        case 0:
            path.lineTo(10 + i * 10, 10 + i * 10);
            break;

        case 1:
            path.cubicTo(10 + i * 100, 10 + i * 100, 10 + i * 100, 10 + i * 100, 10 + i * 100, 10 + i * 100);
            break;

        case 2:
            path.quadTo(10 + i * 10, 10 + i * 10, 10 + i * 10, 10 + i * 10);
            break;
        }
    }
    return path;
}

}

class LayoutInformationTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLayoutInformation();
    void testLayoutMatches();
};

void LayoutInformationTest::testLayoutInformation()
{
    State rootState, copyState;
    rootState.setLabel("root");
    copyState.setLabel("root");
    rootState.setPos(QPointF(123,456));
    rootState.setWidth(1.2);
    rootState.setHeight(3.4);
    State *lastState = nullptr;
    State *lastCopyState = nullptr;
    for (int i = 0; i < 3 ; ++i) {
        State *state = new State();
        state->setLabel(QString("state %1").arg(i));
        state->setPos(QPointF(i, 10 * i));
        state->setWidth(100 + i);
        state->setHeight(50 + i);
        if (lastState) {
            Transition *transition = new Transition(lastState);
            transition->setLabel(QString("trans %1").arg(i));
            transition->setTargetState(static_cast<State*>(state));
            transition->setTargetState(state);
            transition->setPos(QPointF(i * 10, i * 10));
            transition->setLabelBoundingRect(QRectF(10 + i, 10 + i * 10, 10 + i, 10 + i));
            transition->setShape(createPath(transition->pos()));
        }
        lastState = state;

        state = new State();
        state->setLabel(QString("state %1").arg(i));
        if (lastCopyState) {
            Transition *transition = new Transition(lastCopyState);
            transition->setLabel(QString("trans %1").arg(i));
            transition->setTargetState(static_cast<State*>(state));
            transition->setTargetState(state);
        }
        lastCopyState = state;
    }

    QJsonDocument doc(LayoutImportExport::exportLayout(&rootState));
    LayoutImportExport::importLayout(QJsonDocument::fromBinaryData(doc.toBinaryData()).object(), &copyState);
    compare(&rootState, &copyState);
}

void LayoutInformationTest::testLayoutMatches()
{
    {
        // check switching label of an element
        State s1;
        s1.setLabel("s1");

        QJsonDocument doc(LayoutImportExport::exportLayout(&s1));
        QVERIFY(LayoutImportExport::matches(doc.object(), &s1));
        s1.setLabel("s2");
        QVERIFY(!LayoutImportExport::matches(doc.object(), &s1));
    }
    {
        // check keeping ids but turn them into another type (transition <-> state)
        State s1;
        s1.setLabel("s1");

        Transition t1(&s1);
        t1.setLabel("t1");

        QJsonDocument doc(LayoutImportExport::exportLayout(&s1));
        QVERIFY(LayoutImportExport::matches(doc.object(), &s1));
        s1.setLabel("t1");
        t1.setLabel("s1");
        QVERIFY(!LayoutImportExport::matches(doc.object(), &s1));
    }
}

QTEST_MAIN(LayoutInformationTest)
#include "test_layoutinformation.moc"
