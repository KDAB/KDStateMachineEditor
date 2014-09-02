/*
  layerwiselayouter.cpp

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

#include <config-kdsme.h>

#include "layerwiselayouter.h"

#include "graphvizlayout/graphvizlayerlayouter.h"
#include "graphvizlayout/graphvizlayouter.h"
#include "layoutitem.h"
#include "layoutproperties.h"
#include "layoututils.h"
#include "layoutitemwalker.h"
#include "element.h"
#include "view/view.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QFontMetrics>
#include <QRectF>

using namespace KDSME;

namespace {

QRectF boundingRectForCollapsedRegion(StateLayoutItem* state, View* view)
{
    static const QRectF minimumSize(0, 0, 100, 20);

    Q_ASSERT(state);
    if (!state) {
        return minimumSize;
    }

    const LayoutProperties& prop = *(view->layoutProperties());
    const QString label = state->element()->label();
    const QFontMetricsF fm(prop.regionLabelFont());
    const qreal width = fm.width(label);
    const qreal height = fm.height();
    const qreal margin = prop.regionLabelMargins();
    return QRectF(0, 0, width + prop.regionLabelButtonBoxSize().width() + 2*margin, height + 2*margin);
}

}

RegionLayouter::RegionLayouter(QObject* parent)
    : QObject(parent)
{
}

void RegionLayouter::layoutRegion(StateLayoutItem* state, const QRectF& boundingRectHint, View* view)
{
    Q_ASSERT(state);
    if (state->childStates().isEmpty()) {
        return; // no composite state, do nothing
    }

    // Calculate bounding rect on our own in case it is empty
    Q_ASSERT(!boundingRectHint.isEmpty());
    const QRectF boundingRect = boundingRectHint;

    const LayoutProperties& properties = *(view->layoutProperties());

    const qreal fontHeight = properties.regionLabelFont().pointSizeF();
    const qreal regionLabelMargin = properties.regionMargins();
    const qreal regionLabelHeight = regionLabelMargin + fontHeight + regionLabelMargin;
    const qreal innerMargin = properties.regionMargins();

    const QPointF offset = QPointF(innerMargin, innerMargin + regionLabelHeight);
    state->setWidth(boundingRect.width() + 2*innerMargin);
    state->setHeight(boundingRect.height() + 2*innerMargin + regionLabelHeight);
    LayoutUtils::moveInner(state, offset);
}

LayerwiseLayouter::LayerwiseLayouter(QObject* parent)
    : Layouter(parent)
#if HAVE_GRAPHVIZ
    , m_layerLayouter(new GraphvizLayerLayouter(this))
#else
    , m_layerLayouter(nullptr)
#endif
    , m_regionLayouter(new RegionLayouter(this))
    , m_view(0)
{
}

QRectF LayerwiseLayouter::layout(StateLayoutItem* state, View* view)
{
    Q_ASSERT(state);
    m_view = view;

    QElapsedTimer timer;
    timer.start();

    LayoutWalker walker(LayoutWalker::PostOrderTraversal);
    walker.walkItems(state, std::bind(&LayerwiseLayouter::layoutState, this, std::placeholders::_1));

    qDebug() << Q_FUNC_INFO << "Took" << timer.elapsed() << "ms";

    return QRect();
}

LayoutWalker::VisitResult LayerwiseLayouter::layoutState(LayoutItem* item)
{
    StateLayoutItem* state = qobject_cast<StateLayoutItem*>(item);
    if (!state || state->childStates().isEmpty()) {
        return LayoutWalker::RecursiveWalk;
    }

    QRectF boundingRect;
#if HAVE_GRAPHVIZ
    if (state->isExpanded()) {
        boundingRect = m_layerLayouter->layout(state, m_view);
    } else {
        boundingRect = boundingRectForCollapsedRegion(state, m_view);
    }
#else
    // let's just show collapsed states in this case
    boundingRect = boundingRectForCollapsedRegion(state, m_view);
#endif

    m_regionLayouter->layoutRegion(state, boundingRect, m_view);

    return LayoutWalker::RecursiveWalk;
}
