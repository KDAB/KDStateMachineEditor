/*
  test_layoutinformation.cpp

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

#include "element.h"

#include "layoutimportexport.h"
#include "layoutitem.h"
#include "layoututils.h"

#include <QDebug>
#include <QtTest>

using namespace KDSME;

namespace {

void compare(const TransitionLayoutItem *obj1, const TransitionLayoutItem *obj2)
{
    QCOMPARE(obj1->pos(), obj2->pos());
    QCOMPARE(obj1->labelBoundingRect(), obj2->labelBoundingRect());
    QCOMPARE(obj1->shape(), obj2->shape());
}

void compare(const StateLayoutItem *obj1, const StateLayoutItem *obj2)
{
    QCOMPARE(obj1->element()->label(), obj2->element()->label());
    QCOMPARE(obj1->pos(), obj2->pos());
    QCOMPARE(obj1->width(), obj2->width());
    QCOMPARE(obj1->height(), obj2->height());

    auto copyChildren = obj2->childStates();
    foreach (const StateLayoutItem *child, obj1->childStates()) {
        auto copyChild = std::find_if(copyChildren.begin(), copyChildren.end(), [&child](StateLayoutItem *obj)->bool{return obj->element()->label() == child->element()->label();});
        if (copyChild != copyChildren.end())
            compare(child, *copyChild);
    }

    auto copyTransitions = obj2->transitions();
    foreach (const TransitionLayoutItem *item, obj1->transitions()) {
        auto copyTransition = std::find_if(copyTransitions.begin(), copyTransitions.end(), [&item](TransitionLayoutItem *obj)->bool{return obj->element()->label() == item->element()->label();});
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
    StateLayoutItem rootLayoutItem, copyLayoutItem;
    rootLayoutItem.setElement(&rootState);
    copyLayoutItem.setElement(&copyState);

    rootLayoutItem.setPos(QPointF(123,456));
    rootLayoutItem.setWidth(1.2);
    rootLayoutItem.setHeight(3.4);
    StateLayoutItem *lastState = nullptr;
    StateLayoutItem *lastCopyState = nullptr;
    for (int i = 0; i < 3 ; ++i) {
        StateLayoutItem *obj = new StateLayoutItem(&rootLayoutItem);
        obj->setElement(new State(&rootState));
        obj->element()->setLabel(QString("obj %1").arg(i));
        obj->setPos(QPointF(i, 10 * i));
        obj->setWidth(100 + i);
        obj->setHeight(50 + i);
        if (lastState) {
            TransitionLayoutItem *transition = new TransitionLayoutItem(lastState);
            Transition *t = new Transition(static_cast<State*>(lastState->element()));
            t->setLabel(QString("trans %1").arg(i));
            t->setTargetState(static_cast<State*>(obj->element()));
            transition->setElement(t);
            transition->setTargetState(obj);
            transition->setPos(QPointF(i * 10, i * 10));
            transition->setLabelBoundingRect(QRectF(10 + i, 10 + i * 10, 10 + i, 10 + i));
            transition->setShape(createPath(transition->pos()));
        }
        lastState = obj;

        obj = new StateLayoutItem(&copyLayoutItem);
        obj->setElement(new State(&copyState));
        obj->element()->setLabel(QString("obj %1").arg(i));
        if (lastCopyState) {
            TransitionLayoutItem *transition = new TransitionLayoutItem(lastCopyState);
            Transition *t = new Transition(static_cast<State*>(lastCopyState->element()));
            t->setLabel(QString("trans %1").arg(i));
            t->setTargetState(static_cast<State*>(obj->element()));
            transition->setElement(t);
            transition->setTargetState(obj);
        }
        lastCopyState = obj;
    }

    QJsonDocument doc(LayoutImportExport::exportLayout(&rootLayoutItem));
    LayoutImportExport::importLayout(QJsonDocument::fromBinaryData(doc.toBinaryData()).object(), &copyLayoutItem);
    compare(&rootLayoutItem, &copyLayoutItem);
}

void LayoutInformationTest::testLayoutMatches()
{
    {
        // check switching label of an element
        State s1;
        s1.setLabel("s1");
        StateLayoutItem i1;
        i1.setElement(&s1);

        QJsonDocument doc(LayoutImportExport::exportLayout(&i1));
        QVERIFY(LayoutImportExport::matches(doc.object(), &i1));
        s1.setLabel("s2");
        QVERIFY(!LayoutImportExport::matches(doc.object(), &i1));
    }
    {
        // check keeping ids but turn them into another type (transition <-> state)
        State s1;
        s1.setLabel("s1");
        StateLayoutItem si1;
        si1.setElement(&s1);

        Transition t1(&s1);
        t1.setLabel("t1");
        TransitionLayoutItem ti1;
        ti1.setElement(&t1);

        QJsonDocument doc(LayoutImportExport::exportLayout(&si1));
        QVERIFY(LayoutImportExport::matches(doc.object(), &si1));
        s1.setLabel("t1");
        t1.setLabel("s1");
        QVERIFY(!LayoutImportExport::matches(doc.object(), &si1));
    }
}

QTEST_MAIN(LayoutInformationTest)
#include "test_layoutinformation.moc"
