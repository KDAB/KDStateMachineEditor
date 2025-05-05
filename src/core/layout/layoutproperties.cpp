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

#include "layoutproperties.h"

using namespace KDSME;

struct LayoutProperties::Private
{
    Private();

    qreal regionMargins;
    QFont regionLabelFont;
    qreal regionLabelMargins;
    QSizeF regionLabelButtonBoxSize;
    bool showTransitionLabels;
};

LayoutProperties::Private::Private()
    : regionMargins(10.)
    , regionLabelMargins(5.)
    , regionLabelButtonBoxSize(18., 18.)
    , showTransitionLabels(true)
{
    regionLabelFont.setBold(true);
}

LayoutProperties::LayoutProperties(QObject *parent)
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
    Q_EMIT regionMarginsChanged(margins);
    Q_EMIT changed();
}

QFont LayoutProperties::regionLabelFont() const
{
    return d->regionLabelFont;
}

void LayoutProperties::setRegionLabelFont(const QFont &font)
{
    if (d->regionLabelFont == font) {
        return;
    }

    d->regionLabelFont = font;
    Q_EMIT regionLabelFontChanged(font);
    Q_EMIT changed();
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
    Q_EMIT regionLabelMarginsChanged(margins);
    Q_EMIT changed();
}

QSizeF LayoutProperties::regionLabelButtonBoxSize() const
{
    return d->regionLabelButtonBoxSize;
}

void LayoutProperties::setRegionLabelButtonBoxSize(const QSizeF &size) // clazy:exclude=function-args-by-value
{
    if (d->regionLabelButtonBoxSize == size) {
        return;
    }

    d->regionLabelButtonBoxSize = size;
    Q_EMIT regionLabelButtonBoxSizeChanged(size);
    Q_EMIT changed();
}

bool LayoutProperties::showTransitionLabels() const
{
    return d->showTransitionLabels;
}

void LayoutProperties::setShowTransitionLabels(bool show)
{
    if (d->showTransitionLabels == show) {
        return;
    }

    d->showTransitionLabels = show;
    Q_EMIT showTransitionLabelsChanged(show);
    Q_EMIT changed();
}
