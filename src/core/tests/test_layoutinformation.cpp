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

#include "layoutimportexport.h"
#include "layoututils.h"
#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainterPath>
#include <QTest>

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
    const auto childStates = state1->childStates();
    for (const State *child : childStates) {
        auto copyChild = std::find_if(copyChildren.begin(), copyChildren.end(), [&child](State *state) -> bool { return state->label() == child->label(); });
        if (copyChild != copyChildren.end())
            compare(child, *copyChild);
    }

    auto copyTransitions = state2->transitions();
    const auto state1Transitions = state1->transitions();
    for (const Transition *item : state1Transitions) {
        auto copyTransition = std::find_if(copyTransitions.begin(), copyTransitions.end(), [&item](Transition *state) -> bool { return state->label() == item->label(); });
        if (copyTransition != copyTransitions.end()) {
            compare(item, *copyTransition);
        }
    }
}

QPainterPath createPath(QPointF start)
{
    QPainterPath path(start);
    for (int i = 0; i < 3; ++i) {
        switch (i) {
        case 0:
            path.lineTo(10 + (i * 10), 10 + (i * 10));
            break;

        case 1:
            path.cubicTo(10 + (i * 100), 10 + (i * 100), 10 + (i * 100), 10 + (i * 100), 10 + (i * 100), 10 + (i * 100));
            break;

        case 2:
            path.quadTo(10 + (i * 10), 10 + (i * 10), 10 + (i * 10), 10 + (i * 10));
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
    State rootState;
    State copyState;
    rootState.setLabel(QStringLiteral("root"));
    copyState.setLabel(QStringLiteral("root"));
    rootState.setPos(QPointF(123, 456));
    rootState.setWidth(1.2);
    rootState.setHeight(3.4);
    State *lastState = nullptr;
    State *lastCopyState = nullptr;
    for (int i = 0; i < 3; ++i) {
        auto *state = new State();
        state->setLabel(QStringLiteral("state %1").arg(i));
        state->setPos(QPointF(i, 10 * i));
        state->setWidth(100 + i);
        state->setHeight(50 + i);
        if (lastState) {
            auto *transition = new Transition(lastState);
            transition->setLabel(QStringLiteral("trans %1").arg(i));
            transition->setTargetState(static_cast<State *>(state)); // NOLINT(readability-redundant-casting)
            transition->setTargetState(state);
            transition->setPos(QPointF(i * 10, i * 10));
            transition->setLabelBoundingRect(QRectF(10 + i, 10 + (i * 10), 10 + i, 10 + i));
            transition->setShape(createPath(transition->pos()));
        }
        lastState = state;

        state = new State();
        state->setLabel(QStringLiteral("state %1").arg(i));
        if (lastCopyState) {
            auto *transition = new Transition(lastCopyState);
            transition->setLabel(QStringLiteral("trans %1").arg(i));
            transition->setTargetState(static_cast<State *>(state)); // NOLINT(readability-redundant-casting)
            transition->setTargetState(state);
        }
        lastCopyState = state;
    }

    const QJsonObject json(LayoutImportExport::exportLayout(&rootState));

    const QByteArray cbor = QCborValue::fromJsonValue(json).toCbor();
    LayoutImportExport::importLayout(QCborValue::fromCbor(cbor).toJsonValue().toObject(), &copyState);

    compare(&rootState, &copyState);
}

void LayoutInformationTest::testLayoutMatches()
{
    {
        // check switching label of an element
        State s1;
        s1.setLabel(QStringLiteral("s1"));

        const QJsonDocument doc(LayoutImportExport::exportLayout(&s1));
        QVERIFY(LayoutImportExport::matches(doc.object(), &s1));
        s1.setLabel(QStringLiteral("s2"));
        QVERIFY(!LayoutImportExport::matches(doc.object(), &s1));
    }
    {
        // check keeping ids but turn them into another type (transition <-> state)
        State s1;
        s1.setLabel(QStringLiteral("s1"));

        Transition t1(&s1);
        t1.setLabel(QStringLiteral("t1"));

        const QJsonDocument doc(LayoutImportExport::exportLayout(&s1));
        QVERIFY(LayoutImportExport::matches(doc.object(), &s1));
        s1.setLabel(QStringLiteral("t1"));
        t1.setLabel(QStringLiteral("s1"));
        QVERIFY(!LayoutImportExport::matches(doc.object(), &s1));
    }
}

QTEST_MAIN(LayoutInformationTest)
#include "test_layoutinformation.moc"
