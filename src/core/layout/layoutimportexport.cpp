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

#include "element.h"
#include "state.h"
#include "transition.h"

#include "debug.h"

#include <QIODevice>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>

using namespace KDSME;

namespace {

QJsonObject stateLayoutToJson(const State *state)
{
    QJsonObject res;
    res[u"label"] = state->label();
    res[u"x"] = state->pos().x();
    res[u"y"] = state->pos().y();
    res[u"width"] = state->width();
    res[u"height"] = state->height();
    return res;
}

QJsonObject transitionLayoutToJson(const Transition *transition)
{
    QJsonObject res;
    res[u"label"] = transition->label();
    res[u"x"] = transition->pos().x();
    res[u"y"] = transition->pos().y();
    const QRectF labelRect = transition->labelBoundingRect();
    QJsonObject lbr;
    lbr[u"x"] = labelRect.x();
    lbr[u"y"] = labelRect.y();
    lbr[u"width"] = labelRect.width();
    lbr[u"height"] = labelRect.height();
    res[u"labelBoundingRect"] = lbr;
    QByteArray shapeData;
    QDataStream ds(&shapeData, QIODevice::WriteOnly);
    ds << transition->shape();
    res[u"shape"] = QLatin1String(shapeData.toBase64());
    return res;
}

void importStateLayout(const QJsonObject &data, State *state)
{
    const auto x = data.find(u"x");
    if (x == data.end())
        return;

    const auto y = data.find(u"y");
    if (y == data.end())
        return;

    const auto width = data.find(u"width");
    if (width == data.end())
        return;

    const auto height = data.find(u"height");
    if (height == data.end())
        return;

    state->setPos(QPointF((*x).toDouble(), (*y).toDouble()));
    state->setWidth((*width).toDouble());
    state->setHeight((*height).toDouble());
}

bool isValidState(const QJsonObject &data, const State *state)
{
    return data.value(u"label") == state->label() && data.contains(u"x") && data.contains(u"y") && data.contains(u"width") && data.contains(u"height");
}

bool isValidTransition(const QJsonObject &data, const Transition *state)
{
    return data.value(u"label") == state->label() && data.contains(u"x") && data.contains(u"y") && data.contains(u"labelBoundingRect") && data.contains(u"shape");
}

void importTransitionLayout(const QJsonObject &data, Transition *transition)
{
    const auto x = data.find(u"x");
    if (x == data.end())
        return;

    const auto y = data.find(u"y");
    if (y == data.end())
        return;

    const auto lbrIt = data.find(u"labelBoundingRect");
    if (lbrIt == data.end())
        return;

    const auto shape = data.find(u"shape");
    if (shape == data.end())
        return;

    transition->setPos(QPointF((*x).toDouble(), (*y).toDouble()));
    QJsonObject lbr = (*lbrIt).toObject();
    transition->setLabelBoundingRect(QRectF(lbr[u"x"].toDouble(),
                                            lbr[u"y"].toDouble(),
                                            lbr[u"width"].toDouble(),
                                            lbr[u"height"].toDouble()));
    QByteArray shapeData = QByteArray::fromBase64((*shape).toString().toLatin1());
    QPainterPath shapePath;
    QDataStream ds(&shapeData, QIODevice::ReadOnly);
    ds >> shapePath;
    transition->setShape(shapePath);
}

} // anonymous namespace

void LayoutImportExport::importLayout(const QJsonObject &data, State *state)
{
    importStateLayout(data, state);

    const QJsonArray states = data.value(u"childStates").toArray();
    for (int i = 0; i < states.size() && i < state->childStates().size(); ++i) {
        State *child = state->childStates().at(i);
        importLayout(states.at(i).toObject(), child);
    }

    const QJsonArray transitions = data.value(u"transitions").toArray();
    for (int i = 0; i < transitions.size() && i < state->transitions().size(); ++i) {
        Transition *child = state->transitions().at(i);
        importTransitionLayout(transitions.at(i).toObject(), child);
    }
}

QJsonObject LayoutImportExport::exportLayout(const State *state)
{
    QJsonObject res = stateLayoutToJson(state);

    QJsonArray states;
    const auto childStates = state->childStates();
    for (const State *child : childStates) {
        states.push_back(exportLayout(child)); // cppcheck-suppress useStlAlgorithm
    }
    res[u"childStates"] = states;

    QJsonArray transitions;
    const auto stateTransitions = state->transitions();
    for (const Transition *child : stateTransitions) {
        transitions.push_back(transitionLayoutToJson(child)); // cppcheck-suppress useStlAlgorithm
    }
    res[u"transitions"] = transitions;

    return res;
}

bool LayoutImportExport::matches(const QJsonObject &data, State *state) // cppcheck-suppress constParameterPointer
{
    if (!isValidState(data, state))
        return false;

    const QJsonArray states = data.value(u"childStates").toArray();
    if (states.size() != state->childStates().size())
        return false;

    for (int i = 0; i < states.size(); ++i) {
        State *child = state->childStates().at(i);
        if (!matches(states.at(i).toObject(), child))
            return false;
    }

    const QJsonArray transitions = data.value(u"transitions").toArray();
    if (transitions.size() != state->transitions().size())
        return false;

    for (int i = 0; i < transitions.size(); ++i) {
        const Transition *child = state->transitions().at(i);
        if (!isValidTransition(transitions.at(i).toObject(), child))
            return false;
    }

    return true;
}
