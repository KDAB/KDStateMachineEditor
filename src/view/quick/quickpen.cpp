/*
  quickpen.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "quickpen_p.h"

#include <QPen>

QuickPen::QuickPen(QObject* parent)
    : QObject(parent)
    , m_width(0.0)
    , m_color(Qt::black)
    , m_style(Qt::SolidLine)
{
}

void QuickPen::modifyPen(QPen* pen) const
{
    Q_ASSERT(pen);
    pen->setWidthF(m_width);
    pen->setColor(m_color);
    pen->setStyle(m_style);
}

QPen QuickPen::toQPen() const
{
    QPen pen;
    modifyPen(&pen);
    return pen;
}

qreal QuickPen::width() const
{
    return m_width;
}

void QuickPen::setWidth(qreal width)
{
    m_width = width;
    emit penChanged();
}

QColor QuickPen::color() const
{
    return m_color;
}

void QuickPen::setColor(const QColor& color)
{
    m_color = color;
    emit penChanged();
}

Qt::PenStyle QuickPen::style() const
{
    return m_style;
}

void QuickPen::setStyle(Qt::PenStyle style)
{
    m_style = style;
    emit penChanged();
}
