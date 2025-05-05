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

#ifndef KDSME_QUICK_QUICKPAINTERPATH_P_H
#define KDSME_QUICK_QUICKPAINTERPATH_P_H

#include "qopengl2pexvertexarray_p.h"
#include "quickprimitiveitem_p.h"

#include <QObject>
#include <QPainterPath>

QT_BEGIN_NAMESPACE
class QPoint;
QT_END_NAMESPACE

class QuickPainterPathStroker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)

public:
    explicit QuickPainterPathStroker(QObject *parent = nullptr);

    qreal width() const;
    void setWidth(qreal width);

    Q_INVOKABLE QPainterPath createStroke(const QPainterPath &path) const;

Q_SIGNALS:
    void widthChanged(qreal width);

private:
    QPainterPathStroker m_stroker;
};

class QuickPainterPath : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPainterPath path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QPointF startPoint READ startPoint NOTIFY startPointChanged)
    Q_PROPERTY(QPointF endPoint READ endPoint NOTIFY endPointChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged)

public:
    explicit QuickPainterPath(QObject *parent = nullptr);

    QPainterPath path() const;
    void setPath(const QPainterPath &path);

    QPointF startPoint() const;
    QPointF endPoint() const;
    bool isEmpty() const;

    // BEGIN: Replicating QPainterPath API
    Q_INVOKABLE void moveTo(QPointF point);
    Q_INVOKABLE void lineTo(QPointF endPoint);
    Q_INVOKABLE void clear();

    Q_INVOKABLE bool contains(QPointF point) const;
    Q_INVOKABLE qreal angleAtPercent(qreal t) const;
    // END

Q_SIGNALS:
    void pathChanged(const QPainterPath &path);
    void startPointChanged(const QPointF &startPoint);
    void endPointChanged(const QPointF &endPoint);
    void isEmptyChanged(bool isEmpty);

private:
    void updateState(const QPainterPath &path);

    QPainterPath m_path;

    QPointF m_startPoint;
    QPointF m_endPoint;
    bool m_isEmpty;
};

class QuickPainterPathGeometryItem : public QuickGeometryItem
{
    Q_OBJECT

    Q_PROPERTY(QPainterPath path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit QuickPainterPathGeometryItem(QQuickItem *parent = nullptr);

    QPainterPath path() const;
    void setPath(const QPainterPath &path);

Q_SIGNALS:
    void pathChanged(const QPainterPath &path);

private:
    void updateData();

    QOpenGL2PEXVertexArray m_vertexCoordinates;
    QPainterPath m_path;
};

#endif // QUICKPAINTERPATH_P_H
