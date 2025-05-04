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

#include <config-kdsme.h>

#include "layerwiselayouter.h"

#if HAVE_GRAPHVIZ
#include "graphvizlayout/graphvizlayerlayouter.h"
#include "graphvizlayout/graphvizlayouter.h"
#endif
#include "layoutproperties.h"
#include "layoututils.h"
#include "elementwalker.h"
#include "element.h"
#include "state.h"
#include "transition.h"

#include "debug.h"
#include <QElapsedTimer>
#include <QFontMetrics>
#include <QRectF>

using namespace KDSME;

namespace {

QRectF boundingRectForCollapsedRegion(State *state, const LayoutProperties *prop)
{
    static const QRectF minimumSize(0, 0, 100, 20);

    Q_ASSERT(state);
    if (!state) {
        return minimumSize;
    }

    const QString label = state->label();
    const QFontMetricsF fm(prop->regionLabelFont());
    const qreal width = fm.horizontalAdvance(label);
    const qreal height = fm.height();
    const qreal margin = prop->regionLabelMargins();
    return QRectF(0, 0, width + prop->regionLabelButtonBoxSize().width() + 2 * margin, height + 2 * margin);
}

}

RegionLayouter::RegionLayouter(QObject *parent)
    : QObject(parent)
{
}

void RegionLayouter::layoutRegion(State *state, const QRectF &boundingRectHint, const LayoutProperties *properties)
{
    Q_ASSERT(state);
    if (state->childStates().isEmpty()) {
        return; // no composite state, do nothing
    }

    // Calculate bounding rect on our own in case it is empty
    Q_ASSERT(!boundingRectHint.isEmpty());
    const QRectF boundingRect = boundingRectHint;

    const qreal fontHeight = properties->regionLabelFont().pointSizeF();
    const qreal regionLabelMargin = properties->regionMargins();
    const qreal regionLabelHeight = regionLabelMargin + fontHeight + regionLabelMargin;
    const qreal innerMargin = properties->regionMargins();

    const QPointF offset = QPointF(innerMargin, innerMargin + regionLabelHeight);
    state->setWidth(boundingRect.width() + (2 * innerMargin));
    state->setHeight(boundingRect.height() + (2 * innerMargin) + regionLabelHeight);
    LayoutUtils::moveInner(state, offset);
}

LayerwiseLayouter::LayerwiseLayouter(QObject *parent)
    : Layouter(parent)
#if HAVE_GRAPHVIZ
    , m_layerLayouter(new GraphvizLayerLayouter(this))
#else
    , m_layerLayouter(nullptr)
#endif
    , m_regionLayouter(new RegionLayouter(this))
    , m_properties(nullptr)
{
    qCDebug(KDSME_CORE) << "Using" << m_layerLayouter << "as layouter";
}

QRectF LayerwiseLayouter::layout(State *state, const LayoutProperties *properties)
{
    Q_ASSERT(state);
    m_properties = properties;

    ElementWalker walker(ElementWalker::PostOrderTraversal);
    walker.walkItems(state, [&](Element *element) { return layoutState(element); });

    return QRect();
}

ElementWalker::VisitResult LayerwiseLayouter::layoutState(Element *element)
{
    auto *state = qobject_cast<State *>(element);
    if (!state || state->childStates().isEmpty()) {
        return ElementWalker::RecursiveWalk;
    }

    QRectF boundingRect;
#if HAVE_GRAPHVIZ
    if (state->isExpanded()) {
        boundingRect = m_layerLayouter->layout(state, m_properties);
    } else {
        boundingRect = boundingRectForCollapsedRegion(state, m_properties);
    }
#else
    // let's just show collapsed states in this case
    boundingRect = boundingRectForCollapsedRegion(state, m_properties);
#endif

    m_regionLayouter->layoutRegion(state, boundingRect, m_properties);

    return ElementWalker::RecursiveWalk;
}
