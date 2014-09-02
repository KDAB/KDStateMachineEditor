/*
  layoutproperties.cpp

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

#include "layoutproperties.h"

using namespace KDSME;

struct LayoutProperties::Private
{
    Private();

    qreal regionMargins;
    QFont regionLabelFont;
    qreal regionLabelMargins;
    QSizeF regionLabelButtonBoxSize;
};

LayoutProperties::Private::Private()
    : regionMargins(10.)
    , regionLabelMargins(5.)
    , regionLabelButtonBoxSize(18., 18.)
{
    regionLabelFont.setBold(true);
}

LayoutProperties::LayoutProperties(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

LayoutProperties::~LayoutProperties()
{
}

qreal LayoutProperties::regionMargins() const
{
    return d->regionMargins;
}

void LayoutProperties::setRegionMargins(qreal margins)
{
    if (d->regionMargins == margins) {
        return;
    }

    d->regionMargins = margins;
    emit regionMarginsChanged(margins);
}

QFont LayoutProperties::regionLabelFont() const
{
    return d->regionLabelFont;
}

void LayoutProperties::setRegionLabelFont(const QFont& font)
{
    if (d->regionLabelFont == font) {
        return;
    }

    d->regionLabelFont = font;
    emit regionLabelFontChanged(font);
}

qreal LayoutProperties::regionLabelMargins() const
{
    return d->regionLabelMargins;
}

void LayoutProperties::setRegionLabelMargins(qreal margins)
{
    if (d->regionLabelMargins == margins) {
        return;
    }

    d->regionLabelMargins = margins;
    emit regionLabelMarginsChanged(margins);
}

QSizeF LayoutProperties::regionLabelButtonBoxSize() const
{
    return d->regionLabelButtonBoxSize;
}

void LayoutProperties::setRegionLabelButtonBoxSize(const QSizeF& size)
{
    if (d->regionLabelButtonBoxSize == size) {
        return;
    }

    d->regionLabelButtonBoxSize = size;
    emit regionLabelButtonBoxSizeChanged(size);
}
