/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "quickmaskedmousearea_p.h"

#include "debug.h"
#include <QGuiApplication>
#include <QStyleHints>

AbstractMask::AbstractMask(QObject* parent)
    : QObject(parent)
{
}

PainterPathMask::PainterPathMask(QObject* parent)
    : AbstractMask(parent)
{
}

QPainterPath PainterPathMask::path() const
{
    return m_path;
}

void PainterPathMask::setPath(const QPainterPath& path)
{
    if (m_path == path)
        return;

    m_path = path;
    emit pathChanged(m_path);
}

bool PainterPathMask::contains(const QPointF& point) const
{
    return m_path.contains(point);
}

bool PainterPathMask::intersects(const QRectF& rect) const
{
    return m_path.intersects(rect);
}

QuickMaskedMouseArea::QuickMaskedMouseArea(QQuickItem* parent)
    : QQuickItem(parent)
    , m_pressed(false)
    , m_containsMouse(false)
    , m_mask(nullptr)
    , m_tolerance(0.)
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void QuickMaskedMouseArea::setPressed(bool pressed)
{
    if (m_pressed == pressed)
        return;

    m_pressed = pressed;
    emit pressedChanged(m_pressed);
}

void QuickMaskedMouseArea::setContainsMouse(bool containsMouse)
{
    if (m_containsMouse == containsMouse)
        return;

    m_containsMouse = containsMouse;
    emit containsMouseChanged(m_containsMouse);
}

AbstractMask* QuickMaskedMouseArea::mask() const
{
    return m_mask;
}

void QuickMaskedMouseArea::setMask(AbstractMask* mask)
{
    if (m_mask == mask)
        return;

    m_mask = mask;
    emit maskChanged(m_mask);
}

qreal QuickMaskedMouseArea::tolerance() const
{
    return m_tolerance;
}

void QuickMaskedMouseArea::setTolerance(qreal tolerance)
{
    if (qFuzzyCompare(m_tolerance, tolerance)) {
        return;
    }

    m_tolerance = qMax(tolerance, qreal(0.0));
    emit toleranceChanged(m_tolerance);
}

bool QuickMaskedMouseArea::contains(const QPointF& point) const
{
    if (!m_mask) {
        return QQuickItem::contains(point);
    }

    const QRectF rect = boundingRect();
    const bool ignoreBoundingRect = rect.isEmpty();
    if (m_tolerance > 0.) {
        const QRectF fuzzyClickRect(point.x() - m_tolerance, point.y() - m_tolerance,
                                    2*m_tolerance, 2*m_tolerance);
        if (!ignoreBoundingRect && !rect.intersects(fuzzyClickRect)) {
            return false;
        }
        return m_mask->intersects(fuzzyClickRect);
    } else {
        if (!ignoreBoundingRect && !rect.contains(point)) {
            return false;
        }
        return m_mask->contains(point);
    }
    return false; // never reached
}

void QuickMaskedMouseArea::mousePressEvent(QMouseEvent *event)
{
    setPressed(true);
    m_pressPoint = event->pos();
    emit pressed();
}

void QuickMaskedMouseArea::mouseReleaseEvent(QMouseEvent *event)
{
    setPressed(false);
    emit released();


    const int threshold = qApp->styleHints()->startDragDistance();
    const bool isClick = (threshold >= qAbs(event->x() - m_pressPoint.x()) &&
                          threshold >= qAbs(event->y() - m_pressPoint.y()));

    if (isClick)
        emit clicked();
}

void QuickMaskedMouseArea::mouseUngrabEvent()
{
    setPressed(false);
    emit canceled();
}

void QuickMaskedMouseArea::hoverEnterEvent(QHoverEvent *event)
{
    Q_UNUSED(event);
    setContainsMouse(true);
}

void QuickMaskedMouseArea::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event);
    setContainsMouse(false);
}
