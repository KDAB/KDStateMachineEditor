/*
  layerwiselayouter.h

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

#ifndef KDSME_LAYOUT_LAYERWISELAYOUTER_H
#define KDSME_LAYOUT_LAYERWISELAYOUTER_H

#include "layouter.h"
#include "kdsme_core_export.h"
#include "layoutitem.h"
#include "layoutitemwalker.h"

#include <QtCore/qglobal.h>

namespace KDSME {

class GraphvizLayerLayouter;
class StateLayoutItem;

class KDSME_CORE_EXPORT RegionLayouter : public QObject
{
    Q_OBJECT

public:
    explicit RegionLayouter(QObject* parent = nullptr);

    void layoutRegion(StateLayoutItem* state, const QRectF& boundingRectHint, View* view);
};

class KDSME_CORE_EXPORT LayerwiseLayouter : public Layouter
{
    Q_OBJECT

public:
    explicit LayerwiseLayouter(QObject* parent = nullptr);

    virtual QRectF layout(StateLayoutItem* state, View* view) Q_DECL_OVERRIDE;

private:
    LayoutWalker::VisitResult layoutState(LayoutItem* state);

    GraphvizLayerLayouter* m_layerLayouter;
    RegionLayouter* m_regionLayouter;
    View* m_view;
};

}

#endif // LAYERWISELAYOUTER_H
