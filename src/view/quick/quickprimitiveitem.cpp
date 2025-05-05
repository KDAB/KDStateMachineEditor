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

#include "quickprimitiveitem_p.h"

#include <QSGFlatColorMaterial>
#include <QSGNode>

#include <QRectF>

namespace {

QuickGeometryItem::VertexDataType typeOfGeometry(const QSGGeometry *geometry)
{
    switch (geometry->sizeOfVertex()) {
    default:
        Q_ASSERT_X(false, "VertexDataType", "Unknown vertex data type from QSGGeometry");
    case 2 * sizeof(float):
        return QuickGeometryItem::Point2DType;
    case 4 * sizeof(float):
        return QuickGeometryItem::TexturedPoint2DType;
    case 2 * sizeof(float) + 4 * sizeof(char):
        return QuickGeometryItem::ColoredPoint2DType;
    }
}

template<typename T>
QRectF boundingBox_QSGGeometryNode_vertexData(T *vertices, int count)
{
    if (count <= 0) {
        return QRectF();
    }

    float minX = vertices[0].x;
    float minY = vertices[0].y;
    float maxX = vertices[0].x;
    float maxY = vertices[0].y;
    for (int i = 1; i < count; ++i) {
        if (vertices[i].x < minX)
            minX = vertices[i].x;
        if (vertices[i].y < minY)
            minY = vertices[i].y;
        if (vertices[i].x > maxX)
            maxX = vertices[i].x;
        if (vertices[i].y > maxY)
            maxY = vertices[i].y;
    }
    return QRectF(minX, minY, maxX, maxY);
}

}

QRectF GeometryUtils::boundingRect(QSGGeometry *geometry)
{
    switch (typeOfGeometry(geometry)) {
    case QuickGeometryItem::Point2DType:
        return boundingBox_QSGGeometryNode_vertexData(geometry->vertexDataAsPoint2D(), geometry->vertexCount());
    case QuickGeometryItem::ColoredPoint2DType:
        return boundingBox_QSGGeometryNode_vertexData(geometry->vertexDataAsColoredPoint2D(), geometry->vertexCount());
    case QuickGeometryItem::TexturedPoint2DType:
        return boundingBox_QSGGeometryNode_vertexData(geometry->vertexDataAsTexturedPoint2D(), geometry->vertexCount());
    default:
        return QRect();
    }
}

QuickGeometryItem::QuickGeometryItem(QObject *parent)
    : QObject(parent)
    , m_vertexDataType(Point2DType)
    , m_drawingMode(TriangleStrip)
    , m_lineWidth(1.F)
{
}

QList<qreal> QuickGeometryItem::vertexData() const
{
    return m_vertexData;
}

void QuickGeometryItem::setVertexData(const QList<qreal> &vertexData)
{
    if (m_vertexData == vertexData) {
        return;
    }

    m_vertexData = vertexData;
    Q_EMIT vertexDataChanged(m_vertexData);
    Q_EMIT changed();
}

QuickGeometryItem::VertexDataType QuickGeometryItem::vertexDataType() const
{
    return m_vertexDataType;
}

void QuickGeometryItem::setVertexDataType(QuickGeometryItem::VertexDataType type)
{
    if (m_vertexDataType == type)
        return;

    m_vertexDataType = type;
    Q_EMIT vertexDataTypeChanged(m_vertexDataType);
    Q_EMIT changed();
}

QuickGeometryItem::DrawingMode QuickGeometryItem::drawingMode() const
{
    return m_drawingMode;
}

void QuickGeometryItem::setDrawingMode(DrawingMode drawingMode)
{
    if (m_drawingMode == drawingMode)
        return;

    m_drawingMode = drawingMode;
    Q_EMIT drawingModeChanged(m_drawingMode);
    Q_EMIT changed();
}

float QuickGeometryItem::lineWidth() const
{
    return m_lineWidth;
}

void QuickGeometryItem::setLineWidth(float lineWidth)
{
    if (m_lineWidth == lineWidth) {
        return;
    }

    m_lineWidth = lineWidth;
    Q_EMIT lineWidthChanged(m_lineWidth);
    Q_EMIT changed();
}

QSGGeometry *QuickGeometryItem::createGeometry() const
{
    QSGGeometry *geometry = nullptr;
    const QList<qreal> &data = m_vertexData;
    switch (m_vertexDataType) {
    case Point2DType: {
        Q_ASSERT(data.size() % 2 == 0);
        const int vertexCount = static_cast<int>(data.size() / 2);
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount);
        QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
        for (int i = 0; i < vertexCount; ++i) {
            const auto x = static_cast<float>(data[i * 2]);
            const auto y = static_cast<float>(data[(i * 2) + 1]);
            vertices[i].set(x, y);
        }
        break;
    }
    case ColoredPoint2DType:
        // TODO
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0);
        break;
    case TexturedPoint2DType:
        // TODO
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 0);
        break;
    }
    geometry->setDrawingMode(m_drawingMode);
    geometry->setLineWidth(m_lineWidth);
    return geometry;
}

QuickPrimitiveItem::QuickPrimitiveItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_geometryItem(nullptr)
    , m_geometryDirty(false)
    , m_color(Qt::black)
{
    setFlag(ItemHasContents, true);
}

QuickGeometryItem *QuickPrimitiveItem::geometryItem() const
{
    return m_geometryItem;
}

void QuickPrimitiveItem::setGeometryItem(QuickGeometryItem *item)
{
    if (m_geometryItem == item)
        return;

    if (m_geometryItem) {
        disconnect(m_geometryItem, &QuickGeometryItem::changed, this, &QuickPrimitiveItem::updateGeometry);
    }
    m_geometryItem = item;
    if (m_geometryItem) {
        connect(m_geometryItem, &QuickGeometryItem::changed, this, &QuickPrimitiveItem::updateGeometry);
    }
    Q_EMIT geometryItemChanged(m_geometryItem);
    update();
}

void QuickPrimitiveItem::updateImplicitSize(QSGGeometry *geometry)
{
    const QRectF boundingRect = GeometryUtils::boundingRect(geometry);
    setImplicitWidth(boundingRect.x() + boundingRect.width());
    setImplicitHeight(boundingRect.y() + boundingRect.height());
}

QColor QuickPrimitiveItem::color() const
{
    return m_color;
}

void QuickPrimitiveItem::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    Q_EMIT colorChanged(m_color);
    update();
}

void QuickPrimitiveItem::updateGeometry()
{
    m_geometryDirty = true;
    update();
}

QSGNode *QuickPrimitiveItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    if (!geometryItem())
        return nullptr;

    auto *node = static_cast<QSGGeometryNode *>(oldNode);
    if (!node) {
        node = new QSGGeometryNode;
        node->setFlag(QSGNode::OwnsGeometry);
        auto *material = new QSGFlatColorMaterial;
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        m_geometryDirty = true;
    }

    if (m_geometryDirty) {
        QSGGeometry *geometry = geometryItem()->createGeometry();
        updateImplicitSize(geometry);
        node->setGeometry(geometry);

        m_geometryDirty = false;
    }


    auto *material = static_cast<QSGFlatColorMaterial *>(node->material());
    material->setColor(m_color);

    return node;
}
