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

#ifndef KDSME_QUICK_QUICKPEN_P_H
#define KDSME_QUICK_QUICKPEN_P_H

#include <QColor>
#include <QObject>

QT_BEGIN_NAMESPACE
class QPen;
QT_END_NAMESPACE

/**
 * QObject-wrapper for QPen
 *
 * @note There's QQuickPen (in a private header) somewhere in QtDeclarative, but that's very rudimentary
 * No support for overwriting the dash pattern, etc. pp.
 */
class QuickPen : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY penChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY penChanged)
    Q_PROPERTY(Qt::PenStyle style READ style WRITE setStyle NOTIFY penChanged)

public:
    explicit QuickPen(QObject *parent = nullptr);

    qreal width() const;
    void setWidth(qreal width);

    QColor color() const;
    void setColor(const QColor &color);

    Qt::PenStyle style() const;
    void setStyle(Qt::PenStyle style);

    QPen toQPen() const;
    void modifyPen(QPen *pen) const;

Q_SIGNALS:
    void penChanged();

private:
    qreal m_width;
    QColor m_color;
    Qt::PenStyle m_style;
};

#endif // QUICKPEN_P_H
