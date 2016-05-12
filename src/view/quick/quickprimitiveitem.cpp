/*
  quickprimitiveitem.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "quickprimitiveitem_p.h"

#include "debug.h"
#include <QSGFlatColorMaterial>
#include <QSGNode>

#include <QRectF>

namespace {

QuickGeometryItem::VertexDataType typeOfGeometry(const QSGGeometry* geometry)
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
QRectF boundingBox_QSGGeometryNode_vertexData(T* vertices, int count)
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

QRectF GeometryUtils::boundingRect(QSGGeometry* geometry)
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

QuickGeometryItem::QuickGeometryItem(QObject* parent)
    : QObject(parent)
    , m_vertexDataType(Point2DType)
    , m_drawingMode(GL_TRIANGLE_STRIP)
    , m_lineWidth(1.f)
{
}

QList<qreal> QuickGeometryItem::vertexData() const
{
    return m_vertexData;
}

void QuickGeometryItem::setVertexData(const QList<qreal>& vertexData)
{
    if (m_vertexData == vertexData) {
        return;
    }

    m_vertexData = vertexData;
    emit vertexDataChanged(m_vertexData);
    emit changed();
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
    emit vertexDataTypeChanged(m_vertexDataType);
    emit changed();
}

GLenum QuickGeometryItem::drawingMode() const
{
    return m_drawingMode;
}

void QuickGeometryItem::setDrawingMode(GLenum drawingMode)
{
    if (m_drawingMode == drawingMode)
        return;

    m_drawingMode = drawingMode;
    emit drawingModeChanged(m_drawingMode);
    emit changed();
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
    emit lineWidthChanged(m_lineWidth);
    emit changed();
}

QSGGeometry* QuickGeometryItem::createGeometry() const
{
    QSGGeometry* geometry = nullptr;
    const QList<qreal>& data = m_vertexData;
    switch (m_vertexDataType) {
    case Point2DType: {
        Q_ASSERT(data.size() % 2 == 0);
        const int vertexCount = data.size()/2;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount);
        QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
        for (int i = 0; i < vertexCount; ++i) {
            const float x = static_cast<float>(data[i*2]);
            const float y = static_cast<float>(data[i*2+1]);
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

QuickPrimitiveItem::QuickPrimitiveItem(QQuickItem* parent)
    : QQuickItem(parent)
    , m_geometryItem(nullptr)
    , m_geometryDirty(false)
    , m_color(Qt::black)
{
    setFlag(ItemHasContents, true);
}

QuickGeometryItem* QuickPrimitiveItem::geometryItem() const
{
    return m_geometryItem;
}

void QuickPrimitiveItem::setGeometryItem(QuickGeometryItem* geometry)
{
    if (m_geometryItem == geometry)
        return;

    if (m_geometryItem) {
        disconnect(m_geometryItem, &QuickGeometryItem::changed, this, &QuickPrimitiveItem::updateGeometry);
    }
    m_geometryItem = geometry;
    if (m_geometryItem) {
        connect(m_geometryItem, &QuickGeometryItem::changed, this, &QuickPrimitiveItem::updateGeometry);
    }
    emit geometryItemChanged(m_geometryItem);
    update();
}

void QuickPrimitiveItem::updateImplicitSize(QSGGeometry* geometry)
{
    const QRectF boundingRect = GeometryUtils::boundingRect(geometry);
    setImplicitWidth(boundingRect.x() + boundingRect.width());
    setImplicitHeight(boundingRect.y() + boundingRect.height());
}

QColor QuickPrimitiveItem::color() const
{
    return m_color;
}

void QuickPrimitiveItem::setColor(const QColor& color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(m_color);
    update();
}

void QuickPrimitiveItem::updateGeometry()
{
    m_geometryDirty = true;
    update();
}

QSGNode* QuickPrimitiveItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    if (!geometryItem())
        return nullptr;

    QSGGeometryNode *node = static_cast<QSGGeometryNode *>(oldNode);
    if (!node) {
        node = new QSGGeometryNode;
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        m_geometryDirty = true;
    }

    if (m_geometryDirty) {
        QSGGeometry* geometry = geometryItem()->createGeometry();
        updateImplicitSize(geometry);
        node->setGeometry(geometry);

        m_geometryDirty = false;
    }


    QSGFlatColorMaterial* material = static_cast<QSGFlatColorMaterial*>(node->material());
    material->setColor(m_color);

    return node;
}
