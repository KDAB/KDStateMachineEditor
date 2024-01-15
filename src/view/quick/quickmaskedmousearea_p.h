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

#ifndef KDSME_QUICK_QUICKMASKEDMOUSEAREA_P_H
#define KDSME_QUICK_QUICKMASKEDMOUSEAREA_P_H

#include <QObject>
#include <QPainterPath>
#include <QQuickItem>

class AbstractMask : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMask(QObject *parent = nullptr);

    virtual bool contains(const QPointF &point) const = 0;
    virtual bool intersects(const QRectF &rect) const = 0;
};

Q_DECLARE_METATYPE(AbstractMask *)

class PainterPathMask : public AbstractMask
{
    Q_OBJECT
    Q_PROPERTY(QPainterPath path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit PainterPathMask(QObject *parent = nullptr);

    QPainterPath path() const;
    void setPath(const QPainterPath &path);

    bool contains(const QPointF &point) const override;
    bool intersects(const QRectF &rect) const override;

Q_SIGNALS:
    void pathChanged(const QPainterPath &path);

private:
    QPainterPath m_path;
};

class QuickMaskedMouseArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool pressed READ isPressed NOTIFY pressedChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
    Q_PROPERTY(AbstractMask *mask READ mask WRITE setMask NOTIFY maskChanged)
    Q_PROPERTY(qreal tolerance READ tolerance WRITE setTolerance NOTIFY toleranceChanged)

public:
    explicit QuickMaskedMouseArea(QQuickItem *parent = nullptr);

    bool isPressed() const
    {
        return m_pressed;
    }
    bool containsMouse() const
    {
        return m_containsMouse;
    }

    AbstractMask *mask() const;
    void setMask(AbstractMask *mask);

    qreal tolerance() const;
    void setTolerance(qreal tolerance);

    bool contains(const QPointF &point) const override;

Q_SIGNALS:
    void pressed();
    void released();
    void clicked();
    void canceled();
    void pressedChanged(bool pressed);
    void containsMouseChanged(bool containsMouse);
    void maskChanged(AbstractMask *mask);
    void toleranceChanged(qreal tolerance);

protected:
    void setPressed(bool pressed);
    void setContainsMouse(bool containsMouse);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void mouseUngrabEvent() override;

private:
    bool m_pressed;
    bool m_containsMouse;
    QPointF m_pressPoint;
    AbstractMask *m_mask;
    qreal m_tolerance;
};

#endif // QUICKMASKEDMOUSEAREA_P_H
