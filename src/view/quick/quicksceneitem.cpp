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

#include "quicksceneitem_p.h"

#include "abstractscene_p.h"
#include "debug.h"
#include "objecttreemodel.h"
#include "state.h"
#include "statemachinescene_p.h"
#include "transition.h"

#include <QPainterPath>

using namespace KDSME;

namespace {

QLineF::IntersectionType intersects(const QLineF &line1, const QLineF &line2, QPointF *intersectionPoint)
{
    return line1.intersects(line2, intersectionPoint);
}

/**
 * Return the first intersection point of @p line with @p rect
 */
QPointF intersected(const QLineF &line, const QRectF &rect)
{
    QPointF point;
    if (intersects(line, QLineF(rect.topLeft(), rect.topRight()), &point) == QLineF::BoundedIntersection)
        return point;
    if (intersects(line, QLineF(rect.topRight(), rect.bottomRight()), &point) == QLineF::BoundedIntersection)
        return point;
    if (intersects(line, QLineF(rect.bottomRight(), rect.bottomLeft()), &point) == QLineF::BoundedIntersection)
        return point;
    if (intersects(line, QLineF(rect.bottomLeft(), rect.topLeft()), &point) == QLineF::BoundedIntersection)
        return point;
    return point;
}

}

QuickSceneItem::QuickSceneItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_scene(nullptr)
    , m_element(nullptr)
    , m_activeness(0.)
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
}

QuickSceneItem::~QuickSceneItem()
{
}

QQuickItem *QuickSceneItem::itemForElement(Element *element) const
{
    // TODO: Ugly, decouple from ObjectTreeModel
    auto model = qobject_cast<ObjectTreeModel *>(scene()->model());
    auto index = model->indexForObject(element);
    Q_ASSERT(index.isValid());
    auto object = scene()->itemForIndex(index);
    Q_ASSERT(object);
    auto sceneItem = qobject_cast<QQuickItem *>(object);
    Q_ASSERT(sceneItem);
    return sceneItem;
}

void QuickSceneItem::sendClickEvent() // NOLINT(readability-make-member-function-const)
{
    // TODO: Send events to scene instead?
    scene()->setCurrentItem(element());
}

StateMachineScene *QuickSceneItem::scene() const
{
    return m_scene;
}

void QuickSceneItem::setScene(StateMachineScene *scene)
{
    Q_ASSERT(scene);
    Q_ASSERT(!m_scene || scene == m_scene);

    m_scene = scene;
}

Element *QuickSceneItem::element() const
{
    return m_element;
}

QPainterPath QuickSceneItem::shape() const
{
    return m_shape;
}

void QuickSceneItem::setShape(const QPainterPath &shape)
{
    if (m_shape == shape) {
        return;
    }

    m_shape = shape;
    Q_EMIT shapeChanged(m_shape);
}

void QuickSceneItem::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void QuickSceneItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        // note: ideally we'd send the context menu event when we're in the mousePressEvent handler
        // but this leads to unexpected behavior in the QQuickWindow implementation.
        // when showing a context menu (i.e. QMenu::exec) from within mousePressEvent,
        // the QQuickWindow will never get the mouse release event.
        // Internally QQuickWindow then assumes the currently clicked item is
        // still actively handling further mouse input, thus are unable to
        // select other items in the scene until we reset the QQuickWindow's state
        AbstractSceneContextMenuEvent contextMenuEvent(
            QContextMenuEvent::Mouse,
            event->pos(), event->globalPosition().toPoint(), event->modifiers(),
            element());
        QCoreApplication::sendEvent(scene(), &contextMenuEvent);
    }
}

bool QuickSceneItem::contains(const QPointF &point) const
{
    if (m_shape.isEmpty()) {
        return QQuickItem::contains(point);
    }

    return m_shape.contains(point);
}

void QuickSceneItem::setElement(Element *element)
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
        setOpacity(m_element->isVisible());
        connect(m_element, &Element::widthChanged, this, &QQuickItem::setWidth);
        connect(m_element, &Element::heightChanged, this, &QQuickItem::setHeight);
        connect(m_element, &Element::visibleChanged, this, &QQuickItem::setOpacity);
        connect(this, &QQuickItem::opacityChanged, m_element, [=]() {
            m_element->setVisible(opacity() > 0);
        });
    }

    Q_EMIT elementChanged(m_element);
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
    Q_EMIT activenessChanged(m_activeness);
}

QuickStateItem::QuickStateItem(QQuickItem *parent)
    : QuickSceneItem(parent)
{
}

QuickTransitionItem::QuickTransitionItem(QQuickItem *parent)
    : QuickSceneItem(parent)
{
}

void QuickTransitionItem::setElement(Element *element)
{
    if (element == this->element()) {
        return;
    }

    auto transition = qobject_cast<Transition *>(element);
    if (element && !transition) {
        qCWarning(KDSME_VIEW) << "Set invalid element on QuickEdgeItem:" << element;
        return;
    }

    if (auto previousTransition = toTransition()) {
        previousTransition->sourceState()->disconnect(this);
        previousTransition->targetState()->disconnect(this);
        previousTransition->disconnect(this);
    }

    QuickSceneItem::setElement(element);

    if (transition) {
        connect(transition, &Transition::sourceStateChanged,
                this, &QuickTransitionItem::updateSource);
        connect(transition, &Transition::targetStateChanged,
                this, &QuickTransitionItem::updateTarget);
    }

    updateSource();
    updateTarget();
}

void QuickTransitionItem::updatePosition()
{
    if (scene()->viewState() == StateMachineScene::RefreshState) {
        return;
    }

    auto transition = toTransition();
    Q_ASSERT(transition);
    const auto sourceState = transition->sourceState();
    const auto targetState = transition->targetState();
    if (sourceState == targetState)
        return;

    const auto sourceStateItem = itemForElement(sourceState);
    const auto targetStateItem = itemForElement(targetState);

    const QRectF startRect(mapFromItem(sourceStateItem, QPointF(0, 0)),
                           QSizeF(sourceStateItem->width(), sourceStateItem->height()));
    const QRectF endRect(mapFromItem(targetStateItem, QPointF(0, 0)),
                         QSizeF(targetStateItem->width(), targetStateItem->height()));

    // const auto shape = transition->shape();
    const auto labelBoundingRect = transition->labelBoundingRect();

    const auto preliminaryEdge = QLineF(startRect.center(), endRect.center());
    const auto startPoint = intersected(preliminaryEdge, startRect);
    Q_ASSERT(!startPoint.isNull());
    const auto endPoint = intersected(preliminaryEdge, endRect);
    Q_ASSERT(!endPoint.isNull());

    QPainterPath newShape(startPoint);
    newShape.lineTo(endPoint);
    transition->setShape(newShape);

    const auto midPoint = newShape.pointAtPercent(0.5);
    const qreal angle = newShape.angleAtPercent(0.5);
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

void QuickTransitionItem::updateSource()
{
    const auto *transition = toTransition();
    if (!transition)
        return;

    disconnect(m_sourceStateConnection);
    if (const auto *source = toTransition()->sourceState()) {
        m_sourceStateConnection = connect(source, &Element::posChanged,
                                          this, &QuickTransitionItem::updatePosition);
    }
}

void QuickTransitionItem::updateTarget()
{
    const auto *transition = toTransition();
    if (!transition)
        return;

    disconnect(m_targetStateConnection);
    if (const auto *targetState = toTransition()->targetState()) {
        m_targetStateConnection = connect(targetState, &Element::posChanged,
                                          this, &QuickTransitionItem::updatePosition);
    }
}

Transition *QuickTransitionItem::toTransition() const
{
    return static_cast<Transition *>(element());
}
