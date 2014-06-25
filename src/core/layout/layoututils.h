/*
  layoututils.h

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

#ifndef KDSME_LAYOUT_LAYOUTUTILS_H
#define KDSME_LAYOUT_LAYOUTUTILS_H

#include "kdsme_core_export.h"

class QPointF;
class QSizeF;
class QString;

namespace KDSME {

class LayoutItem;
class StateLayoutItem;

class KDSME_CORE_EXPORT LayoutUtils
{
public:
    static QSizeF sizeForLabel(const QString& label);

    /// Move all direct children of state @p state by offset @p offset
    static bool moveInner(StateLayoutItem* state, const QPointF& offset);

    static bool fixupLayout(StateLayoutItem* state);

    /**
     * Moves item @p item to the new parent @p parentItem
     *
     * Keeps the item at the same position as before, by translating its
     * KDSME::Item::pos to the new parent's coordinate system
     */
    static bool moveToParent(LayoutItem* item, LayoutItem* parentItem);
};

}

#endif // LAYOUTUTILS_H
