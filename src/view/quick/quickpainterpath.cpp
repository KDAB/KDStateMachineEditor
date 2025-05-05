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

#include "quickpainterpath_p.h"

QuickPainterPathStroker::QuickPainterPathStroker(QObject *parent)
    : QObject(parent)
{
}

qreal QuickPainterPathStroker::width() const
{
    return m_stroker.width();
}

void QuickPainterPathStroker::setWidth(qreal width)
{
    if (qFuzzyCompare(m_stroker.width(), width))
        return;

    m_stroker.setWidth(width);
    Q_EMIT widthChanged(width);
}

QPainterPath QuickPainterPathStroker::createStroke(const QPainterPath &path) const
{
    return m_stroker.createStroke(path);
}

QuickPainterPath::QuickPainterPath(QObject *parent)
    : QObject(parent)
    , m_isEmpty(true)
{
}

QPainterPath QuickPainterPath::path() const
{
    return m_path;
}

void QuickPainterPath::setPath(const QPainterPath &path)
{
    if (m_path == path)
        return;

    m_path = path;
    Q_EMIT pathChanged(path);

    updateState(path);
}

QPointF QuickPainterPath::startPoint() const
{
    return m_startPoint;
}

QPointF QuickPainterPath::endPoint() const
{
    return m_endPoint;
}

bool QuickPainterPath::isEmpty() const
{
    return m_isEmpty;
}

void QuickPainterPath::moveTo(QPointF point)
{
    m_path.moveTo(point);
    updateState(m_path);
}

void QuickPainterPath::lineTo(QPointF endPoint)
{
    m_path.lineTo(endPoint);
    updateState(m_path);
}

void QuickPainterPath::clear()
{
    setPath(QPainterPath());
}

bool QuickPainterPath::contains(QPointF point) const
{
    return m_path.contains(point);
}

qreal QuickPainterPath::angleAtPercent(qreal t) const
{
    return m_path.angleAtPercent(t);
}

void QuickPainterPath::updateState(const QPainterPath &path)
{
    const QPointF newStartPoint = path.pointAtPercent(0.0);
    if (newStartPoint != m_startPoint) {
        m_startPoint = newStartPoint;
        Q_EMIT startPointChanged(m_startPoint);
    }
    const QPointF newEndPoint = path.pointAtPercent(1.0);
    if (newEndPoint != m_endPoint) {
        m_endPoint = newEndPoint;
        Q_EMIT endPointChanged(m_endPoint);
    }
    const bool newIsEmpty = path.isEmpty();
    if (newIsEmpty != m_isEmpty) {
        m_isEmpty = newIsEmpty;
        Q_EMIT isEmptyChanged(m_isEmpty);
    }
}

QuickPainterPathGeometryItem::QuickPainterPathGeometryItem(QQuickItem *parent)
    : QuickGeometryItem(parent)
{
    setDrawingMode(LineStrip);
}


QPainterPath QuickPainterPathGeometryItem::path() const
{
    return m_path;
}

void QuickPainterPathGeometryItem::setPath(const QPainterPath &path)
{
    if (m_path == path)
        return;

    m_path = path;
    updateData();
    Q_EMIT pathChanged(m_path);
    Q_EMIT changed();
}

void QuickPainterPathGeometryItem::updateData()
{
    // update data
    m_vertexCoordinates.clear();
    if (!m_path.isEmpty()) {
        m_vertexCoordinates.addPath(qtVectorPathForPath(m_path), 1.);
    }
    QOpenGLPoint *points = m_vertexCoordinates.data();
    const int vertexCount = m_vertexCoordinates.vertexCount();
    QList<qreal> vertexData;
    vertexData.reserve(vertexCount * 2);
    for (int i = 0; i < vertexCount; ++i) {
        vertexData << points[i].x;
        vertexData << points[i].y;
    }
    setVertexData(vertexData);
}
