/*
  element.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Sebastian Sauer <sebastian.sauer@kdab.com>

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

#include "element.h"
#include "element_p.h"

#include "elementutil.h"
#include "state.h"

#include "objecthelper.h"
#include "layoututils.h"

#include "debug.h"
#include <QEvent>

using namespace KDSME;

Element::Element(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

Element::~Element()
{
}

Element::Type Element::type() const
{
    return ElementType;
}

Element::Flags Element::flags() const
{
    return d->m_flags;
}

void Element::setFlags(Element::Flags flags)
{
    if (d->m_flags == flags)
        return;
    d->m_flags = flags;
    emit flagsChanged(flags);
}

QString Element::label() const
{
    return d->m_label;
}

void Element::setLabel(const QString& label)
{
    if (d->m_label == label)
        return;

    d->m_label = label;
    emit labelChanged(label);
}

quintptr Element::internalId() const
{
    return d->m_id;
}

void Element::setInternalId(quintptr id)
{
    d->m_id = id;
}

void* Element::internalPointer() const
{
    return reinterpret_cast<void*>(d->m_id);
}

void Element::setInternalPointer(void* ptr)
{
    d->m_id = reinterpret_cast<quintptr>(ptr);
}

QPointF Element::pos() const
{
    return d->m_pos;
}

void Element::setPos(const QPointF& pos)
{
    if (d->m_pos == pos)
        return;

    d->m_pos = pos;
    emit posChanged(pos);
}

qreal Element::width() const
{
    return d->m_width;
}

void Element::setWidth(qreal width)
{
    if (d->m_width == width)
        return;

    d->m_width = width;
    emit widthChanged(width);
}

qreal Element::height() const
{
    return d->m_height;
}

void Element::setHeight(qreal height)
{
    if (d->m_height == height)
        return;

    d->m_height = height;
    emit heightChanged(height);
}

QPointF Element::absolutePos() const
{
    QPointF point;
    const Element* current = this;
    do {
        point += current->pos();
        current = current->parentElement();
    } while (current);
    return point;
}

bool Element::isVisible() const
{
    return d->m_visible;
}

void Element::setVisible(bool visible)
{
    if (d->m_visible == visible)
        return;

    d->m_visible = visible;
    emit visibleChanged(d->m_visible);
}

bool Element::isSelected() const
{
    return d->m_selected;
}

void Element::setSelected(bool selected)
{
    if (d->m_selected == selected)
        return;

    d->m_selected = selected;
    emit selectedChanged(d->m_selected);
}

QSizeF Element::preferredSize() const
{
    QSizeF size;
    switch (type())
    {
    case Element::HistoryStateType:
        size = QSizeF(32, 32);
        break;
    case Element::StateType:
        size = LayoutUtils::sizeForLabel(label());
        break;
    case Element::StateMachineType:
        size = boundingRect().size().expandedTo(LayoutUtils::sizeForLabel(label()));
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

QRectF Element::boundingRect() const
{
    const QRectF rect(pos().x(), pos().y(), width(), height());
    return rect;
}

Element* Element::parentElement() const
{
    return qobject_cast<Element*>(parent());
}

QList<Element*> Element::childElements() const
{
    return ObjectHelper::copy_if_type<Element*>(children());
}

QString Element::toDisplayString() const
{
    const QString str = ObjectHelper::className(this, ObjectHelper::StripNameSpace);
    if (label().isEmpty()) {
        return str;
    }
    return QString("%1 [Label: %2]").arg(str).arg(label());
}

const char* Element::typeToString(Element::Type type)
{
    return ObjectHelper::enumToString(&staticMetaObject, "Type", type);
}

Element::Type Element::stringToType(const char* type)
{
    const int value = ObjectHelper::stringToEnum(&staticMetaObject, "Type", type);
    return static_cast<Element::Type>(value);
}
