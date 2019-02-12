/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef KDSME_MODEL_ELEMENT_P_H
#define KDSME_MODEL_ELEMENT_P_H

#include "element.h"

#include <QPointF>
#include <QRectF>
#include <QSizeF>

namespace KDSME {

struct Element::Private
{
    Private()
        : m_id(0)
        , m_flags(ElementIsDragEnabled | ElementIsSelectable | ElementIsEditable)
        , m_visible(true)
        , m_selected(false)
        , m_height(0.0)
        , m_width(0.0)
    {}

    QString m_label;
    quintptr m_id;
    Element::Flags m_flags;

    bool m_visible;
    bool m_selected;

    QPointF m_pos;
    qreal m_height, m_width;
};

}

#endif
