/*
  layoutitem.cpp

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

#include "layoutitem.h"
#include "layoututils.h"

#include "element.h"
#include "objecthelper.h"

#include <QDebug>

using namespace KDSME;

namespace {

QSizeF sizeHintForElement(Element* element)
{
    if (!element)
        return QSizeF();

    switch (element->type())
    {
    case Element::StateMachineType:
        return QSizeF(1024, 1024);
    case Element::FinalStateType:
        return QSizeF(32, 32);
    case Element::HistoryStateType:
    case Element::StateType:
        return LayoutUtils::sizeForLabel(element->label());
    default:
        return QSizeF();
    }
}

}

LayoutItem::LayoutItem(QObject* parent)
    : QObject(parent)
    , m_width(0.0), m_height(0.0)
    , m_visible(true)
    , m_selected(false)
    , m_view(nullptr)
    , m_element(nullptr)
{
}

LayoutItem::~LayoutItem()
{
}

LayoutItem::Type LayoutItem::type() const
{
    return LayoutType;
}

QPointF LayoutItem::pos() const
{
    return m_pos;
}

void LayoutItem::setPos(const QPointF& pos)
{
    if (m_pos == pos)
        return;

    m_pos = pos;
    emit posChanged(pos);
}

qreal LayoutItem::width() const
{
    return m_width;
}

void LayoutItem::setWidth(qreal width)
{
    if (m_width == width)
        return;

    m_width = width;
    emit widthChanged(width);
}

qreal LayoutItem::height() const
{
    return m_height;
}

void LayoutItem::setHeight(qreal height)
{
    if (m_height == height)
        return;

    m_height = height;
    emit heightChanged(height);
}

QPointF LayoutItem::absolutePos() const
{
    QPointF point;
    const LayoutItem* current = this;
    do {
        point += current->pos();
        current = current->parentItem();
    } while (current);
    return point;
}

bool LayoutItem::isVisible() const
{
    return m_visible;
}

void LayoutItem::setVisible(bool visible)
{
    if (m_visible == visible)
        return;

    m_visible = visible;
    emit visibleChanged(m_visible);
}

bool LayoutItem::isSelected() const
{
    return m_selected;
}

void LayoutItem::setSelected(bool selected)
{
    if (m_selected == selected)
        return;

    m_selected = selected;
    emit selectedChanged(m_selected);
}

QRectF LayoutItem::boundingRect() const
{
    const QRectF rect(pos().x(), pos().y(), width(), height());
    return rect;
}

View* LayoutItem::view() const
{
    return m_view;
}

void LayoutItem::setView(View* view)
{
    m_view = view;
}

Element* LayoutItem::element() const
{
    return m_element;
}

void LayoutItem::setElement(Element* element)
{
    if (m_element == element)
        return;

    Element* oldElement = m_element;
    m_element = element;
    elementChanged(oldElement, element);

    emit elementChanged(element);
}

bool LayoutItem::isSiblingOf(LayoutItem* item) const
{
    return parent() ? parent()->children().contains(item) : false;
}

QList<LayoutItem*> LayoutItem::childItems() const
{
    return ObjectHelper::copy_if_type<LayoutItem*>(children());
}

LayoutItem* LayoutItem::parentItem() const
{
    return qobject_cast<LayoutItem*>(parent());
}

QString LayoutItem::toDisplayString() const
{
    const QString str = ObjectHelper::className(this, ObjectHelper::StripNameSpace);
    if (!element())
        return str;
    return QString("%1 (Label: %2]").arg(str).arg(element()->label());
}

void LayoutItem::prepareGeometryChange()
{
    // TODO: notify view
}

void LayoutItem::elementChanged(Element* oldElement, Element* newElement)
{
    Q_UNUSED(oldElement);
    Q_UNUSED(newElement);
}

QDebug operator<<(QDebug dbg, const LayoutItem& item)
{
    dbg.nospace() << "LayoutItem["
        << "pos=(" << item.pos().x() << "," << item.pos().y() << ")"
        << "]";
    return dbg.space();
}

StateLayoutItem::StateLayoutItem(StateLayoutItem* parent)
    : LayoutItem(parent)
    , m_expanded(true)
{
}

LayoutItem::Type StateLayoutItem::type() const
{
    return StateType;
}

bool StateLayoutItem::isExpanded() const
{
    return m_expanded;
}

void StateLayoutItem::setExpanded(bool expanded)
{
    if (m_expanded == expanded)
        return;

    m_expanded = expanded;
    emit expandedChanged(m_expanded);
}

StateLayoutItem* StateLayoutItem::parentState() const
{
    return qobject_cast<StateLayoutItem*>(parent());
}

QList<StateLayoutItem*> StateLayoutItem::childStates() const
{
    return ObjectHelper::copy_if_type<StateLayoutItem*>(children());
}

QList<TransitionLayoutItem*> StateLayoutItem::transitions() const
{
    return ObjectHelper::copy_if_type<TransitionLayoutItem*>(children());
}

void StateLayoutItem::elementChanged(Element* oldElement, Element* newElement)
{
    KDSME::LayoutItem::elementChanged(oldElement, newElement);

    if (oldElement) {
        disconnect(oldElement, &Element::labelChanged, this, &StateLayoutItem::updateItem);
    }
    if (newElement) {
        connect(newElement, &Element::labelChanged, this, &StateLayoutItem::updateItem);
    }
    updateItem();
}

void StateLayoutItem::updateItem()
{
    if (!element())
        return;

    const QSizeF oldSize = QSizeF(height(), width());
    const QSizeF newSize = sizeHintForElement(element());
    if (oldSize == newSize)
        return;

    prepareGeometryChange();
    setWidth(newSize.width());
    setHeight(newSize.height());
}

TransitionLayoutItem::TransitionLayoutItem(StateLayoutItem* parent)
    : LayoutItem(parent)
    , m_targetState(nullptr)
{
}

LayoutItem::Type TransitionLayoutItem::type() const
{
    return TransitionType;
}

QPainterPath TransitionLayoutItem::shape() const
{
    return m_shape;
}

void TransitionLayoutItem::setShape(const QPainterPath& shape)
{
    if (m_shape == shape)
        return;

    m_shape = shape;
    emit shapeChanged(shape);
}

QRectF TransitionLayoutItem::labelBoundingRect() const
{
    return m_labelBoundingRect;
}

void TransitionLayoutItem::setLabelBoundingRect(const QRectF& rect)
{
    if (m_labelBoundingRect == rect)
        return;

    m_labelBoundingRect = rect;
    emit labelBoundingRectChanged(rect);
}

void TransitionLayoutItem::moveToState(StateLayoutItem* state)
{
    qDebug() << Q_FUNC_INFO << this << "to" << state;
    setParent(state);
}

StateLayoutItem* TransitionLayoutItem::sourceState() const
{
    return qobject_cast<StateLayoutItem*>(parent());
}

StateLayoutItem* TransitionLayoutItem::targetState() const
{
    return m_targetState;
}

void TransitionLayoutItem::setTargetState(StateLayoutItem* state)
{
    if (m_targetState == state)
        return;

    m_targetState = state;
    emit targetStateChanged(state);
}

QRectF TransitionLayoutItem::boundingRect() const
{
    return m_shape.boundingRect();
}

void TransitionLayoutItem::elementChanged(Element* oldElement, Element* newElement)
{
    KDSME::LayoutItem::elementChanged(oldElement, newElement);

    if (oldElement) {
        disconnect(oldElement, &Element::labelChanged, this, &TransitionLayoutItem::updateItem);
    }
    if (newElement) {
        connect(newElement, &Element::labelChanged, this, &TransitionLayoutItem::updateItem);
    }
    updateItem();
}

void TransitionLayoutItem::updateItem()
{
    Transition* transition = this->transition();
    if (!transition || transition->targetState())
        return;

    prepareGeometryChange();
    QPainterPath path;
    path.lineTo(50., 50.);
    setShape(path);
}

Transition* TransitionLayoutItem::transition() const
{
    return qobject_cast<Transition*>(element());
}

QDebug operator<<(QDebug dbg, const TransitionLayoutItem& item)
{
    dbg.nospace() << "TransitionLayoutItem["
        << "pos=(" << item.pos().x() << "," << item.pos().y() << ")"
        << ", path length=" << item.shape().length()
        << ", path length=" << item.shape().length()
        << "]";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const StateLayoutItem& item)
{
    dbg.nospace() << "StateLayoutItem["
        << "pos=(" << item.pos().x() << "," << item.pos().y() << ")"
        << ", size=(" << item.width() << "," << item.height() << ")"
        << "]";
    return dbg.space();
}
