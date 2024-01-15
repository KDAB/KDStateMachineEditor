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

#ifndef KDSME_LAYOUT_LAYOUTUTILS_H
#define KDSME_LAYOUT_LAYOUTUTILS_H

#include "kdsme_core_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QPointF;
class QSizeF;
class QString;
QT_END_NAMESPACE

namespace KDSME {

class Element;
class State;

class KDSME_CORE_EXPORT LayoutUtils
{
public:
    static QSizeF sizeForLabel(const QString &label);

    /// Move all direct children of state @p state by offset @p offset
    static bool moveInner(State *state, const QPointF &offset);

    static bool fixupLayout(State *);

    /**
     * Moves item @p item to the new parent @p parentItem
     *
     * Keeps the item at the same position as before, by translating its
     * KDSME::Item::pos to the new parent's coordinate system
     */
    static bool moveToParent(Element *item, Element *parentItem);
};

}

#endif // LAYOUTUTILS_H
