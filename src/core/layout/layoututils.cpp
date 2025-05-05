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

#include "layoututils.h"

#include "element.h"
#include "state.h"
#include "transition.h"

#include <QGuiApplication>
#include "debug.h"
#include <QFontMetricsF>
#include <QSizeF>

using namespace KDSME;

QSizeF LayoutUtils::sizeForLabel(const QString &label)
{
    // TODO: Get LayoutProperties here
    const qreal margin = 10;

    // TODO: Improve
    const QFontMetricsF fm(QGuiApplication::font());
    const qreal width = fm.horizontalAdvance(label);
    return QSizeF(width + (2 * margin), fm.height() + (2 * margin));
}

bool LayoutUtils::moveInner(State *state, const QPointF &offset) // cppcheck-suppress constParameterPointer // clazy:exclude=function-args-by-value
{
    if (!state) {
        return false;
    }

    const auto childStates = state->childStates();
    for (State *childState : childStates) {
        childState->setPos(childState->pos() + offset);
    }
    return true;
}

bool LayoutUtils::fixupLayout(State * /*state*/)
{
    /*
    LayoutWalker walker;
    walker.walkItems(state, [&](Element* obj) -> LayoutWalker::VisitResult {
        return LayoutWalker::RecursiveWalk;
    });
    */
    return true;
}

bool LayoutUtils::moveToParent(Element *item, Element *parentItem)
{
    if (!item || !parentItem)
        return false;

    const auto *oldParent = item->parentElement();
    if (oldParent == parentItem)
        return false; // do nothing

    // map item coordinates to new parent item's coordinate system
    const QPointF oldParentPos = (oldParent ? oldParent->absolutePos() : QPointF(0, 0));
    const QPointF newParentPos = parentItem->absolutePos();
    const QPointF delta = oldParentPos - newParentPos;

    item->setParent(parentItem);
    const QPointF itemRelPos = item->pos();
    item->setPos(itemRelPos + delta);
    return true;
}
