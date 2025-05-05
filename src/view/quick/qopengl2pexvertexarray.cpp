/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: https://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qopengl2pexvertexarray_p.h"

#include <private/qbezier_p.h>

QT_BEGIN_NAMESPACE

void QOpenGL2PEXVertexArray::clear()
{
    vertexArray.reset();
    vertexArrayStops.reset();
    boundingRectDirty = true;
}


QOpenGLRect QOpenGL2PEXVertexArray::boundingRect() const
{
    if (boundingRectDirty) {
        return QOpenGLRect(0.0, 0.0, 0.0, 0.0);
    }

    return QOpenGLRect(minX, minY, maxX, maxY);
}

void QOpenGL2PEXVertexArray::addClosingLine(int index)
{
    const QPointF point(vertexArray.at(index));
    if (point != QPointF(vertexArray.last())) {
        vertexArray.add(QOpenGLPoint{point});
    }
}

void QOpenGL2PEXVertexArray::addCentroid(const QVectorPath &path, int subPathIndex)
{
    const auto *const points = reinterpret_cast<const QPointF *>(path.points());
    const QPainterPath::ElementType *const elements = path.elements();

    QPointF sum = points[subPathIndex];
    int count = 1;

    for (int i = subPathIndex + 1; i < path.elementCount() && (!elements || elements[i] != QPainterPath::MoveToElement); ++i) {
        sum += points[i];
        ++count;
    }

    const QPointF centroid = sum / qreal(count);
    vertexArray.add(QOpenGLPoint{centroid});
}

void QOpenGL2PEXVertexArray::addPath(const QVectorPath &path, GLfloat curveInverseScale, bool outline)  // NOLINT(readability-function-cognitive-complexity)
{
    const auto* const points = reinterpret_cast<const QPointF*>(path.points());
    const QPainterPath::ElementType* const elements = path.elements();

    if (boundingRectDirty) {
        minX = maxX = static_cast<float>(points[0].x());
        minY = maxY = static_cast<float>(points[0].y());
        boundingRectDirty = false;
    }

    if (!outline && !path.isConvex())
        addCentroid(path, 0);

    int lastMoveTo = static_cast<int>(vertexArray.size());
    vertexArray.add(QOpenGLPoint{points[0]}); // The first element is always a moveTo

    do {
        if (!elements) {
//             qDebug("QVectorPath has no elements");
            // If the path has a null elements pointer, the elements implicitly
            // start with a moveTo (already added) and continue with lineTos:
            for (int i=1; i<path.elementCount(); ++i) {
                lineToArray(points[i]);
            }

            break;
        }
//         qDebug("QVectorPath has element types");

        for (int i=1; i<path.elementCount(); ++i) {
            switch (elements[i]) {
            case QPainterPath::MoveToElement:
                if (!outline)
                    addClosingLine(lastMoveTo);
//                qDebug("element[%d] is a MoveToElement", i);
                vertexArrayStops.add(static_cast<int>(vertexArray.size()));
                if (!outline) {
                    if (!path.isConvex()) {
                        addCentroid(path, i);
                    }
                    lastMoveTo = static_cast<int>(vertexArray.size());
                }
                lineToArray(points[i]); // Add the moveTo as a new vertex
                break;
            case QPainterPath::LineToElement:
//                qDebug("element[%d] is a LineToElement", i);
                lineToArray(points[i]);
                break;
            case QPainterPath::CurveToElement: {
                const QBezier b = QBezier::fromPoints(*(static_cast<const QPointF *>(points) + i - 1),
                                                points[i],
                                                points[i+1],
                                                points[i+2]);
                const QRectF bounds = b.bounds();
                // threshold based on same algorithm as in qtriangulatingstroker.cpp
                // NOLINTNEXTLINE
                int threshold = qMin<float>(64, qMax(bounds.width(), bounds.height()) * 3.14F / (curveInverseScale * 6));
                threshold = std::max(threshold, 3);
                const qreal one_over_threshold_minus_1 = qreal(1) / (threshold - 1);
                for (int t=0; t<threshold; ++t) {
                    const QPointF pt = b.pointAt(t * one_over_threshold_minus_1);
                    lineToArray(pt);
                }
                i += 2;
                break; }
            default:
                break;
            }
        }
    } while (0);

    if (!outline)
        addClosingLine(lastMoveTo);
    vertexArrayStops.add(static_cast<int>(vertexArray.size()));
}

void QOpenGL2PEXVertexArray::lineToArray(QPointF point)
{
    const auto x = static_cast<float>(point.x());
    const auto y = static_cast<float>(point.y());
    vertexArray.add(QOpenGLPoint{x, y});

    if (x > maxX)
        maxX = x;
    else if (x < minX)
        minX = x;
    if (y > maxY)
        maxY = y;
    else if (y < minY)
        minY = y;
}

QT_END_NAMESPACE
