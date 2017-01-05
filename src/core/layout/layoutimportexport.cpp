/*
  layoutimportexport.cpp

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

#include "element.h"
#include "state.h"
#include "transition.h"

#include "debug.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>

using namespace KDSME;

namespace {

QJsonObject stateLayoutToJson(const State* state)
{
    QJsonObject res;
    res["label"] = state->label();
    res["x"] = state->pos().x();
    res["y"] = state->pos().y();
    res["width"] = state->width();
    res["height"] = state->height();
    return res;
}

QJsonObject transitionLayoutToJson(const Transition* transition)
{
    QJsonObject res;
    res["label"] = transition->label();
    res["x"] = transition->pos().x();
    res["y"] = transition->pos().y();
    QRectF labelRect = transition->labelBoundingRect();
    QJsonObject lbr;
    lbr["x"] = labelRect.x();
    lbr["y"] = labelRect.y();
    lbr["width"] = labelRect.width();
    lbr["height"] = labelRect.height();
    res["labelBoundingRect"] = lbr;
    QByteArray shapeData;
    QDataStream ds(&shapeData, QIODevice::WriteOnly);
    ds << transition->shape();
    res["shape"] = QLatin1String(shapeData.toBase64());
    return res;
}

void importStateLayout(const QJsonObject& data, State* state)
{
    QJsonObject::const_iterator x = data.find("x");
    if (x == data.end())
        return;

    QJsonObject::const_iterator y = data.find("y");
    if (y == data.end())
        return;

    QJsonObject::const_iterator width = data.find("width");
    if (width == data.end())
        return;

    QJsonObject::const_iterator height = data.find("height");
    if (height == data.end())
        return;

    state->setPos(QPointF((*x).toDouble(), (*y).toDouble()));
    state->setWidth((*width).toDouble());
    state->setHeight((*height).toDouble());
}

bool isValidState(const QJsonObject& data, State *state)
{
    return
        data.value("label") == state->label() &&
        data.contains("x") &&
        data.contains("y") &&
        data.contains("width") &&
        data.contains("height");
}

bool isValidTransition(const QJsonObject& data, Transition *state)
{
    return
        data.value("label") == state->label() &&
        data.contains("x") &&
        data.contains("y") &&
        data.contains("labelBoundingRect") &&
        data.contains("shape");
}

void importTransitionLayout(const QJsonObject& data, Transition* transition)
{
    QJsonObject::const_iterator x = data.find("x");
    if (x == data.end())
        return;

    QJsonObject::const_iterator y = data.find("y");
    if (y == data.end())
        return;

    QJsonObject::const_iterator lbrIt = data.find("labelBoundingRect");
    if (lbrIt == data.end())
        return;

    QJsonObject::const_iterator shape = data.find("shape");
    if (shape == data.end())
        return;

    transition->setPos(QPointF((*x).toDouble(), (*y).toDouble()));
    QJsonObject lbr = (*lbrIt).toObject();
    transition->setLabelBoundingRect(QRectF(lbr["x"].toDouble(),
                                 lbr["y"].toDouble(),
                                 lbr["width"].toDouble(),
                                 lbr["height"].toDouble()));
    QByteArray shapeData = QByteArray::fromBase64((*shape).toString().toLatin1());
    QPainterPath shapePath;
    QDataStream ds(&shapeData, QIODevice::ReadOnly);
    ds >> shapePath;
    transition->setShape(shapePath);
}

} // anonymous namespace

void LayoutImportExport::importLayout(const QJsonObject &rootData, State *state)
{
    importStateLayout(rootData, state);

    const QJsonArray states = rootData.value("childStates").toArray();
    for (int i = 0; i < states.size() && i < state->childStates().size(); ++i) {
        State *child = state->childStates().at(i);
        importLayout(states.at(i).toObject(), child);
    }

    const QJsonArray transitions = rootData.value("transitions").toArray();
    for (int i = 0; i < transitions.size() && i < state->transitions().size(); ++i) {
        Transition *child = state->transitions().at(i);
        importTransitionLayout(transitions.at(i).toObject(), child);
    }
}

QJsonObject LayoutImportExport::exportLayout(const State *state)
{
    QJsonObject res = stateLayoutToJson(state);

    QJsonArray states;
    foreach (State *child, state->childStates())
        states.push_back(exportLayout(child));
    res["childStates"] = states;

    QJsonArray transitions;
    foreach (Transition *child, state->transitions())
        transitions.push_back(transitionLayoutToJson(child));
    res["transitions"] = transitions;

    return res;
}

bool LayoutImportExport::matches(const QJsonObject& data, State* state)
{
    if (!isValidState(data, state))
        return false;

    const QJsonArray states = data.value("childStates").toArray();
    if (states.size() != state->childStates().size())
        return false;

    for (int i = 0; i < states.size(); ++i) {
        State *child = state->childStates().at(i);
        if (!matches(states.at(i).toObject(), child))
            return false;
    }

    const QJsonArray transitions = data.value("transitions").toArray();
    if (transitions.size() != state->transitions().size())
        return false;

    for (int i = 0; i < transitions.size(); ++i) {
        Transition *child = state->transitions().at(i);
        if (!isValidTransition(transitions.at(i).toObject(), child))
            return false;
    }

    return true;
}
