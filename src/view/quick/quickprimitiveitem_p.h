/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_QUICK_QUICKPRIMITIVEITEM_P_H
#define KDSME_QUICK_QUICKPRIMITIVEITEM_P_H

#include <QScopedPointer>
#include <QtQuick/QSGGeometry>
#include <QtQuick/QQuickItem>

QT_BEGIN_NAMESPACE
class QRectF;
QT_END_NAMESPACE

namespace GeometryUtils {

QRectF boundingRect(QSGGeometry *geometry);

}

class QuickGeometryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> data READ vertexData WRITE setVertexData NOTIFY vertexDataChanged)
    Q_PROPERTY(QuickGeometryItem::VertexDataType type READ dataType READ vertexDataType WRITE setVertexDataType NOTIFY vertexDataTypeChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

    Q_PROPERTY(DrawingMode drawingMode READ drawingMode READ drawingMode WRITE setDrawingMode NOTIFY drawingModeChanged)

public:
    enum VertexDataType
    {
        Point2DType, ///< Default
        ColoredPoint2DType,
        TexturedPoint2DType
    };
    Q_ENUM(VertexDataType)

    enum DrawingMode
    {
        Points = QSGGeometry::DrawPoints,
        Lines = QSGGeometry::DrawLines,
        LineLoop = QSGGeometry::DrawLineLoop,
        LineStrip = QSGGeometry::DrawLineStrip,
        Triangles = QSGGeometry::DrawTriangles,
        TriangleStrip = QSGGeometry::DrawTriangleStrip,
        TriangleFan = QSGGeometry::DrawTriangleFan
    };
    Q_ENUM(DrawingMode)

    explicit QuickGeometryItem(QObject *parent = nullptr);

    QList<qreal> vertexData() const;
    void setVertexData(const QList<qreal> &vertexData);

    VertexDataType vertexDataType() const;
    void setVertexDataType(VertexDataType type);

    DrawingMode drawingMode() const;
    void setDrawingMode(DrawingMode drawingMode);

    float lineWidth() const;
    void setLineWidth(float lineWidth);

    QSGGeometry *createGeometry() const;

Q_SIGNALS:
    void vertexDataChanged(const QList<qreal> &vertexData);
    void vertexDataTypeChanged(QuickGeometryItem::VertexDataType type);
    void drawingModeChanged(QuickGeometryItem::DrawingMode drawingMode);
    void lineWidthChanged(float lineWidth);
    /// Emitted in case any of this object properties changes
    void changed();

private:
    QList<qreal> m_vertexData;
    VertexDataType m_vertexDataType;
    DrawingMode m_drawingMode;
    float m_lineWidth;
};

Q_DECLARE_METATYPE(QuickGeometryItem::VertexDataType)
Q_DECLARE_METATYPE(QuickGeometryItem::DrawingMode)

class QuickPrimitiveItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QuickGeometryItem *geometry READ geometryItem WRITE setGeometryItem NOTIFY geometryItemChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit QuickPrimitiveItem(QQuickItem *parent = nullptr);

    QuickGeometryItem *geometryItem() const;
    void setGeometryItem(QuickGeometryItem *item);

    QColor color() const;
    void setColor(const QColor &color);

Q_SIGNALS:
    void geometryItemChanged(QuickGeometryItem *geometry);
    void colorChanged(const QColor &color);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

private Q_SLOTS:
    void updateGeometry();

private:
    void updateImplicitSize(QSGGeometry *geometry);

    QuickGeometryItem *m_geometryItem;
    bool m_geometryDirty;
    QColor m_color;
};

#endif // QUICKPRIMITIVEITEM_P_H
