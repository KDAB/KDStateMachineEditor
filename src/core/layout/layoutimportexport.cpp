/*
  layoutimportexport.cpp

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

#include "layoutimportexport.h"

#include "element.h"
#include "layoutitem.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

using namespace KDSME;

namespace {

QJsonObject stateLayoutToJson(const StateLayoutItem* stateItem)
{
    QJsonObject res;
    res["label"] = stateItem->element()->label();
    res["x"] = stateItem->pos().x();
    res["y"] = stateItem->pos().y();
    res["width"] = stateItem->width();
    res["height"] = stateItem->height();
    return res;
}

QJsonObject transitionLayoutToJson(const TransitionLayoutItem* layoutItem)
{
    QJsonObject res;
    res["label"] = layoutItem->element()->label();
    res["x"] = layoutItem->pos().x();
    res["y"] = layoutItem->pos().y();
    QRectF labelRect = layoutItem->labelBoundingRect();
    QJsonObject lbr;
    lbr["x"] = labelRect.x();
    lbr["y"] = labelRect.y();
    lbr["width"] = labelRect.width();
    lbr["height"] = labelRect.height();
    res["labelBoundingRect"] = lbr;
    QByteArray shapeData;
    QDataStream ds(&shapeData, QIODevice::WriteOnly);
    ds << layoutItem->shape();
    res["shape"] = QLatin1String(shapeData.toBase64());
    return res;
}

void importStateLayout(const QJsonObject& data, StateLayoutItem* stateItem)
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

    stateItem->setPos(QPointF((*x).toDouble(), (*y).toDouble()));
    stateItem->setWidth((*width).toDouble());
    stateItem->setHeight((*height).toDouble());
}

bool isValidState(const QJsonObject& data, StateLayoutItem *item)
{
    return
        data.value("label") == item->element()->label() &&
        data.contains("x") &&
        data.contains("y") &&
        data.contains("width") &&
        data.contains("height");
}

bool isValidTransition(const QJsonObject& data, TransitionLayoutItem *item)
{
    return
        data.value("label") == item->element()->label() &&
        data.contains("x") &&
        data.contains("y") &&
        data.contains("labelBoundingRect") &&
        data.contains("shape");
}

void importTransitionLayout(const QJsonObject& data, TransitionLayoutItem* transitionItem)
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

    transitionItem->setPos(QPointF((*x).toDouble(), (*y).toDouble()));
    QJsonObject lbr = (*lbrIt).toObject();
    transitionItem->setLabelBoundingRect(QRectF(lbr["x"].toDouble(),
                                 lbr["y"].toDouble(),
                                 lbr["width"].toDouble(),
                                 lbr["height"].toDouble()));
    QByteArray shapeData = QByteArray::fromBase64((*shape).toString().toLatin1());
    QPainterPath shapePath;
    QDataStream ds(&shapeData, QIODevice::ReadOnly);
    ds >> shapePath;
    transitionItem->setShape(shapePath);
}

} // anonymous namespace

void LayoutImportExport::importLayout(const QJsonObject &rootData, StateLayoutItem *state)
{
    importStateLayout(rootData, state);

    const QJsonArray states = rootData.value("childStates").toArray();
    for (int i = 0; i < states.size() && i < state->childStates().size(); ++i) {
        StateLayoutItem *item = state->childStates().at(i);
        importLayout(states.at(i).toObject(), item);
    }

    const QJsonArray transitions = rootData.value("transitions").toArray();
    for (int i = 0; i < transitions.size() && i < state->transitions().size(); ++i) {
        TransitionLayoutItem *item = state->transitions().at(i);
        importTransitionLayout(transitions.at(i).toObject(), item);
    }
}

QJsonObject LayoutImportExport::exportLayout(const StateLayoutItem *rootItem)
{
    QJsonObject res = stateLayoutToJson(rootItem);

    QJsonArray states;
    foreach (StateLayoutItem *item, rootItem->childStates())
        states.push_back(exportLayout(item));
    res["childStates"] = states;

    QJsonArray transitions;
    foreach (TransitionLayoutItem *item, rootItem->transitions())
        transitions.push_back(transitionLayoutToJson(item));
    res["transitions"] = transitions;

    return res;
}

bool LayoutImportExport::matches(const QJsonObject& data, StateLayoutItem* state)
{
    if (!isValidState(data, state))
        return false;

    const QJsonArray states = data.value("childStates").toArray();
    if (states.size() != state->childStates().size())
        return false;

    for (int i = 0; i < states.size(); ++i) {
        StateLayoutItem *item = state->childStates().at(i);
        if (!matches(states.at(i).toObject(), item))
            return false;
    }

    const QJsonArray transitions = data.value("transitions").toArray();
    if (transitions.size() != state->transitions().size())
        return false;

    for (int i = 0; i < transitions.size(); ++i) {
        TransitionLayoutItem *item = state->transitions().at(i);
        if (!isValidTransition(transitions.at(i).toObject(), item))
            return false;
    }

    return true;
}
