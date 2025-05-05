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

#include "element.h"
#include "element_p.h"

#include "elementutil.h"
#include "state.h"

#include "objecthelper.h"
#include "layoututils.h"

#include "debug.h"
#include <QEvent>

using namespace KDSME;

Element::Element(QObject *parent)
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
    Q_EMIT flagsChanged(flags);
}

QString Element::label() const
{
    return d->m_label;
}

void Element::setLabel(const QString &label)
{
    if (d->m_label == label)
        return;

    d->m_label = label;
    Q_EMIT labelChanged(label);
}

quintptr Element::internalId() const
{
    return d->m_id;
}

void Element::setInternalId(quintptr id)
{
    d->m_id = id;
}

void *Element::internalPointer() const
{
    return reinterpret_cast<void *>(d->m_id);
}

void Element::setInternalPointer(void *ptr)
{
    d->m_id = reinterpret_cast<quintptr>(ptr);
}

QPointF Element::pos() const
{
    return d->m_pos;
}

void Element::setPos(const QPointF &pos) // clazy:exclude=function-args-by-value
{
    if (d->m_pos == pos)
        return;

    d->m_pos = pos;
    Q_EMIT posChanged(pos);
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
    Q_EMIT widthChanged(width);
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
    Q_EMIT heightChanged(height);
}

QPointF Element::absolutePos() const
{
    QPointF point;
    const Element *current = this;
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
    Q_EMIT visibleChanged(d->m_visible);
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
    Q_EMIT selectedChanged(d->m_selected);
}

QSizeF Element::preferredSize() const
{
    QSizeF size;
    switch (type()) {
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
    case Element::SignalTransitionType:
    case Element::TimeoutTransitionType:
    case Element::PseudoStateType:
    case Element::TransitionType:
        break;
    }
    return size;
}

QRectF Element::boundingRect() const
{
    const QRectF rect(pos().x(), pos().y(), width(), height());
    return rect;
}

Element *Element::parentElement() const
{
    return qobject_cast<Element *>(parent());
}

void Element::setParentElement(Element *parent)
{
    setParent(static_cast<QObject *>(parent));
}

QList<Element *> Element::childElements() const
{
    return ObjectHelper::copy_if_type<Element *>(children());
}

QString Element::toDisplayString() const
{
    QString str = ObjectHelper::className(this, ObjectHelper::StripNameSpace);
    if (label().isEmpty()) {
        return str;
    }
    return QStringLiteral("%1 [Label: %2]").arg(str).arg(label()); // clazy:exclude=qstring-arg
}

const char *Element::typeToString(Element::Type type)
{
    return ObjectHelper::enumToString(&staticMetaObject, "Type", type);
}

Element::Type Element::stringToType(const char *type)
{
    const int value = ObjectHelper::stringToEnum(&staticMetaObject, "Type", type);
    return static_cast<Element::Type>(value);
}

void Element::setParent(QObject *object)
{
    const auto *oldElementParent = parentElement();
    const auto newElementParent = qobject_cast<Element *>(object);
    if (oldElementParent != newElementParent) {
        Q_EMIT parentChanged(newElementParent);
    }

    QObject::setParent(object);
}

#include "moc_element.cpp"
