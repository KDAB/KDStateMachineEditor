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

struct LayoutItem::Private
{
    Private();

    QPointF m_pos;
    qreal m_height, m_width;
    bool m_visible;
    bool m_selected;

    View* m_view;
    Element* m_element;
};

LayoutItem::Private::Private()
    : m_height(0.0)
    , m_width(0.0)
    , m_visible(true)
    , m_selected(false)
    , m_view(nullptr)
    , m_element(nullptr)
{
}

LayoutItem::LayoutItem(QObject* parent)
    : QObject(parent)
    , d(new Private)
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
    return d->m_pos;
}

void LayoutItem::setPos(const QPointF& pos)
{
    if (d->m_pos == pos)
        return;

    d->m_pos = pos;
    emit posChanged(pos);
}

qreal LayoutItem::width() const
{
    return d->m_width;
}

void LayoutItem::setWidth(qreal width)
{
    if (d->m_width == width)
        return;

    d->m_width = width;
    emit widthChanged(width);
}

qreal LayoutItem::height() const
{
    return d->m_height;
}

void LayoutItem::setHeight(qreal height)
{
    if (d->m_height == height)
        return;

    d->m_height = height;
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
    return d->m_visible;
}

void LayoutItem::setVisible(bool visible)
{
    if (d->m_visible == visible)
        return;

    d->m_visible = visible;
    emit visibleChanged(d->m_visible);
}

bool LayoutItem::isSelected() const
{
    return d->m_selected;
}

void LayoutItem::setSelected(bool selected)
{
    if (d->m_selected == selected)
        return;

    d->m_selected = selected;
    emit selectedChanged(d->m_selected);
}

QSizeF LayoutItem::preferredSize() const
{
    if (!element())
        return QSizeF();

    QSizeF size;
    switch (element()->type())
    {
    case Element::HistoryStateType:
        size = QSizeF(32, 32);
        break;
    case Element::StateType:
        size = LayoutUtils::sizeForLabel(element()->label());
        break;
    case Element::StateMachineType:
        size = boundingRect().size().expandedTo(LayoutUtils::sizeForLabel(element()->label()));
        break;
    case Element::FinalStateType:
        size = QSizeF(32, 32);
        break;
    case Element::ElementType:
    case Element::TransitionType:
    case Element::SignalTransitionType:
    case Element::TimeoutTransitionType:
    case Element::PseudoStateType:
        break;
    }
    return size;
}

QRectF LayoutItem::boundingRect() const
{
    const QRectF rect(pos().x(), pos().y(), width(), height());
    return rect;
}

View* LayoutItem::view() const
{
    return d->m_view;
}

void LayoutItem::setView(View* view)
{
    d->m_view = view;
}

Element* LayoutItem::element() const
{
    return d->m_element;
}

void LayoutItem::setElement(Element* element)
{
    if (d->m_element == element)
        return;

    Element* oldElement = d->m_element;
    d->m_element = element;
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

QDebug KDSME::operator<<(QDebug dbg, const LayoutItem& item)
{
    dbg.nospace() << "LayoutItem["
        << "pos=(" << item.pos().x() << "," << item.pos().y() << ")"
        << "]";
    return dbg.space();
}

struct StateLayoutItem::Private
{
    Private();

    bool m_expanded;
};

StateLayoutItem::Private::Private()
    : m_expanded(true)
{

}

StateLayoutItem::StateLayoutItem(StateLayoutItem* parent)
    : LayoutItem(parent)
    , d(new Private)
{
}

StateLayoutItem::~StateLayoutItem()
{
}

LayoutItem::Type StateLayoutItem::type() const
{
    return StateType;
}

bool StateLayoutItem::isExpanded() const
{
    return d->m_expanded;
}

void StateLayoutItem::setExpanded(bool expanded)
{
    if (d->m_expanded == expanded)
        return;

    d->m_expanded = expanded;
    emit expandedChanged(d->m_expanded);
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

struct TransitionLayoutItem::Private
{
    Private(TransitionLayoutItem* q);

    TransitionLayoutItem* q;

    KDSME::Transition* transition() const;

    QPainterPath m_shape;
    QRectF m_labelBoundingRect;
    StateLayoutItem* m_targetState;
};

TransitionLayoutItem::Private::Private(TransitionLayoutItem* q)
    : q(q)
    , m_targetState(nullptr)
{
}

TransitionLayoutItem::TransitionLayoutItem(StateLayoutItem* parent)
    : LayoutItem(parent)
    , d(new Private(this))
{
}

TransitionLayoutItem::~TransitionLayoutItem()
{
}

LayoutItem::Type TransitionLayoutItem::type() const
{
    return TransitionType;
}

QPainterPath TransitionLayoutItem::shape() const
{
    return d->m_shape;
}

void TransitionLayoutItem::setShape(const QPainterPath& shape)
{
    if (d->m_shape == shape)
        return;

    d->m_shape = shape;
    emit shapeChanged(shape);
}

QRectF TransitionLayoutItem::labelBoundingRect() const
{
    return d->m_labelBoundingRect;
}

void TransitionLayoutItem::setLabelBoundingRect(const QRectF& rect)
{
    if (d->m_labelBoundingRect == rect)
        return;

    d->m_labelBoundingRect = rect;
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
    return d->m_targetState;
}

void TransitionLayoutItem::setTargetState(StateLayoutItem* state)
{
    if (d->m_targetState == state)
        return;

    d->m_targetState = state;
    emit targetStateChanged(state);
}

QRectF TransitionLayoutItem::boundingRect() const
{
    return d->m_shape.boundingRect();
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
    Transition* transition = d->transition();
    if (!transition || transition->targetState())
        return;

    prepareGeometryChange();
    QPainterPath path;
    path.lineTo(50., 50.);
    setShape(path);
}

Transition* TransitionLayoutItem::Private::transition() const
{
    return qobject_cast<Transition*>(q->element());
}

QDebug KDSME::operator<<(QDebug dbg, const TransitionLayoutItem& item)
{
    dbg.nospace() << "TransitionLayoutItem["
        << "pos=(" << item.pos().x() << "," << item.pos().y() << ")"
        << ", path length=" << item.shape().length()
        << ", path length=" << item.shape().length()
        << "]";
    return dbg.space();
}

QDebug KDSME::operator<<(QDebug dbg, const StateLayoutItem& item)
{
    dbg.nospace() << "StateLayoutItem["
        << "pos=(" << item.pos().x() << "," << item.pos().y() << ")"
        << ", size=(" << item.width() << "," << item.height() << ")"
        << "]";
    return dbg.space();
}
