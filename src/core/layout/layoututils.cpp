/*
  layoututils.cpp

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

#include "layoututils.h"

#include "element.h"
#include "state.h"
#include "transition.h"

#include <QGuiApplication>
#include "debug.h"
#include <QFontMetricsF>
#include <QSizeF>

using namespace KDSME;

QSizeF LayoutUtils::sizeForLabel(const QString& label)
{
    // TODO: Get LayoutProperties here
    const qreal margin = 10;

    // TODO: Improve
    QFontMetricsF fm(QGuiApplication::font());
    const qreal width = fm.width(label);
    return QSizeF(width + 2*margin, fm.height() + 2*margin);
}

bool LayoutUtils::moveInner(State* state, const QPointF& offset)
{
    if (!state) {
        return false;
    }

    foreach (State* childState, state->childStates()) {
        childState->setPos(childState->pos() + offset);
    }
    return true;
}

bool LayoutUtils::fixupLayout(State* /*state*/)
{
    /*
    LayoutWalker walker;
    walker.walkItems(state, [&](Element* obj) -> LayoutWalker::VisitResult {
        return LayoutWalker::RecursiveWalk;
    });
    */
    return true;
}

bool LayoutUtils::moveToParent(Element* obj, Element* newParent)
{
    if (!obj || !newParent)
        return false;

    auto oldParent = obj->parentElement();
    if (oldParent == newParent)
        return false; // do nothing

    // map item coordinates to new parent item's coordinate system
    const QPointF oldParentPos = (oldParent ? oldParent->absolutePos() : QPointF(0, 0));
    const QPointF newParentPos = newParent->absolutePos();
    const QPointF delta = oldParentPos - newParentPos;

    obj->setParent(newParent);
    const QPointF itemRelPos = obj->pos();
    obj->setPos(itemRelPos + delta);
    return true;
}
