/*
  quicksceneitem.cpp

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

#include "quicksceneitem.h"

#include "debug.h"
#include "objecttreemodel.h"
#include "state.h"
#include "statemachinescene.h"
#include "statemachinescene_p.h"
#include "transition.h"

#include <QPainterPath>

using namespace KDSME;

QuickSceneItem::QuickSceneItem(QQuickItem* parent)
    : QQuickItem(parent)
    , m_scene(nullptr)
    , m_element(nullptr)
    , m_activeness(0.)
{
}

QuickSceneItem::~QuickSceneItem()
{
}

QQuickItem* QuickSceneItem::itemForElement(Element* element) const
{
    // TODO: Ugly, decouple from ObjectTreeModel
    auto model = qobject_cast<ObjectTreeModel*>(scene()->model());
    auto index = model->indexForObject(element);
    Q_ASSERT(index.isValid());
    auto object = scene()->itemForIndex(index);
    Q_ASSERT(object);
    auto sceneItem = qobject_cast<QQuickItem*>(object);
    Q_ASSERT(sceneItem);
    return sceneItem;
}

void QuickSceneItem::sendClickEvent()
{
    // TODO: Send events to scene instead?
    scene()->setCurrentItem(element());
}

StateMachineScene* QuickSceneItem::scene() const
{
    return m_scene;
}

void QuickSceneItem::setScene(StateMachineScene* scene)
{
    Q_ASSERT(scene);
    Q_ASSERT(!m_scene);

    m_scene = scene;
}

Element* QuickSceneItem::element() const
{
    return m_element;
}

void QuickSceneItem::setElement(Element* element)
{
    if (m_element == element)
        return;

    if (m_element) {
        m_element->disconnect(this);
    }

    m_element = element;

    if (m_element) {
        setWidth(m_element->width());
        setHeight(m_element->height());
        setVisible(m_element->isVisible());
        connect(m_element, &Element::widthChanged, this, &QQuickItem::setWidth);
        connect(m_element, &Element::heightChanged, this, &QQuickItem::setHeight);
        connect(m_element, &Element::visibleChanged, this, &QQuickItem::setVisible);
    }

    emit elementChanged(m_element);
}

qreal QuickSceneItem::activeness() const
{
    return m_activeness;
}

void QuickSceneItem::setActiveness(qreal activeness)
{
    if (qFuzzyCompare(m_activeness, activeness))
        return;

    m_activeness = activeness;
    emit activenessChanged(m_activeness);
}

QuickStateItem::QuickStateItem(QQuickItem* parent): QuickSceneItem(parent)
{
}

QuickTransitionItem::QuickTransitionItem(QQuickItem* parent): QuickSceneItem(parent)
{
}

void QuickTransitionItem::setElement(Element* element)
{
    if (element == this->element()) {
        return;
    }

    auto transition = qobject_cast<Transition*>(element);
    if (element && !transition) {
        qWarning() << "Set invalid element on QuickEdgeItem:" << element;
        return;
    }

    if (transition) {
        transition->sourceState()->disconnect(this);
        transition->sourceState()->disconnect(this);
        transition->disconnect(this);
    }

    if (transition->sourceState())
        connect(transition->sourceState(), &Element::posChanged,
            this, &QuickTransitionItem::updatePosition);
    if (transition->targetState())
        connect(transition->targetState(), &Element::posChanged,
            this, &QuickTransitionItem::updatePosition);

    QuickSceneItem::setElement(element);
}

void QuickTransitionItem::updatePosition()
{
    if (scene()->state() == StateMachineScene::RefreshState) {
        return;
    }

    auto transition = toTransition();
    const auto targetState = transition->targetState();
    const auto targetStateItem = itemForElement(targetState);

    const QRectF endRect(mapFromItem(targetStateItem, {0, 0}),
                         QSizeF(targetStateItem->width(), targetStateItem->height()));

    if (transition->label() == "e4") {
        qDebug() << transition->shape();
    }

    const auto shape = transition->shape();
    const auto labelBoundingRect = transition->labelBoundingRect();
    const auto startPoint = shape.pointAtPercent(0);

    QPainterPath rectPath;
    rectPath.addRect(endRect);
    QPainterPath linePath(startPoint);
    linePath.lineTo(endRect.center());
    QPainterPathStroker stroker;
    const auto filledLine = stroker.createStroke(linePath);
    const auto endPoint = rectPath.intersected(filledLine).pointAtPercent(0);

    QPainterPath newShape(startPoint);
    newShape.lineTo(endPoint);
    transition->setShape(newShape);

    const auto midPoint = linePath.pointAtPercent(0.5);
    const qreal angle = linePath.angleAtPercent(0.5);
    QRectF newLabelBoundingRect(labelBoundingRect);
    if (angle < 90) {
        newLabelBoundingRect.moveTopLeft(midPoint);
    } else if (angle < 180) {
        newLabelBoundingRect.moveBottomLeft(midPoint);
    } else if (angle < 270) {
        newLabelBoundingRect.moveBottomRight(midPoint);
    } else {
        newLabelBoundingRect.moveTopRight(midPoint);
    }
    transition->setLabelBoundingRect(newLabelBoundingRect);
}

Transition* QuickTransitionItem::toTransition() const
{
    return static_cast<Transition*>(element());
}
