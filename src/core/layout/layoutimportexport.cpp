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

#include <QJsonObject>

using namespace KDSME;

namespace {

QJsonObject stateLayoutToJson(const StateLayoutItem* stateItem)
{
    QJsonObject res;
    res["x"] = stateItem->pos().x();
    res["y"] = stateItem->pos().y();
    res["width"] = stateItem->width();
    res["height"] = stateItem->height();
    return res;
}

QJsonObject transitionLayoutToJson(const TransitionLayoutItem* layoutItem)
{
    QJsonObject res;
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

bool isValidState(const QJsonObject& data, StateLayoutItem*)
{
    return
        data.contains("x") &&
        data.contains("y") &&
        data.contains("width") &&
        data.contains("height");
}

bool isValidTransition(const QJsonObject& data, TransitionLayoutItem*)
{
    return
        data.contains("x") &&
        data.contains("y") &&
        data.contains("labelBoundingRect") &&
        data.contains("shape");
}

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

void exportLayout(QJsonObject &root, const StateLayoutItem *rootItem)
{
    root[rootItem->element()->label()] = stateLayoutToJson(rootItem);

    foreach (const TransitionLayoutItem *item, rootItem->transitions()) {
        root[item->element()->label()] = transitionLayoutToJson(item);
    }

    foreach (const StateLayoutItem *item, rootItem->childStates()) {
        exportLayout(root, item);
    }
}

void LayoutImportExport::importLayout(const QJsonObject &rootData, StateLayoutItem *state)
{
    QJsonObject::const_iterator dataIt = rootData.find(state->element()->label());
    if (dataIt == rootData.end())
        return;

    QJsonObject data((*dataIt).toObject());
    importStateLayout(data, state);

    foreach (StateLayoutItem *item, state->childStates())
        importLayout(rootData, item);

    foreach (TransitionLayoutItem *item, state->transitions()) {
        QJsonObject::const_iterator transition = rootData.find(item->element()->label());
        if (transition != rootData.end())
            importTransitionLayout(transition.value().toObject(), item);
    }
}

QJsonObject LayoutImportExport::exportLayout(const StateLayoutItem *rootItem)
{
    QJsonObject res;
    exportLayout(res, rootItem);
    return res;
}

bool LayoutImportExport::matches(const QJsonObject& data, StateLayoutItem* state)
{
    auto it = data.find(state->element()->label());
    if (it == data.constEnd()) {
        return false;
    }

    if (!isValidState((*it).toObject(), state))
        return false;

    foreach (StateLayoutItem *item, state->childStates()) {
        if (!matches(data, item)) {
            return false;
        }
    }

    foreach (TransitionLayoutItem *item, state->transitions()) {
        auto it = data.find(item->element()->label());
        if (it == data.constEnd())
            return false;

        if (!isValidTransition((*it).toObject(), item))
            return false;
    }
    return true;
}
