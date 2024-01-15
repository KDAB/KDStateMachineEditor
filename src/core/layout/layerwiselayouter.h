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

#ifndef KDSME_LAYOUT_LAYERWISELAYOUTER_H
#define KDSME_LAYOUT_LAYERWISELAYOUTER_H

#include "kdsme_core_export.h"

#include "layouter.h"
#include "elementwalker.h"

#include <QtCore/qglobal.h>

namespace KDSME {

class GraphvizLayerLayouter;
class LayoutProperties;

class KDSME_CORE_EXPORT RegionLayouter : public QObject
{
    Q_OBJECT

public:
    explicit RegionLayouter(QObject *parent = nullptr);

    void layoutRegion(State *state, const QRectF &boundingRectHint, const LayoutProperties *properties);
};

class KDSME_CORE_EXPORT LayerwiseLayouter : public Layouter
{
    Q_OBJECT

public:
    explicit LayerwiseLayouter(QObject *parent = nullptr);

    QRectF layout(State *state, const LayoutProperties *properties) override;

private:
    ElementWalker::VisitResult layoutState(Element *element);

    GraphvizLayerLayouter *m_layerLayouter;
    RegionLayouter *m_regionLayouter;
    const LayoutProperties *m_properties;
};

}

#endif // LAYERWISELAYOUTER_H
