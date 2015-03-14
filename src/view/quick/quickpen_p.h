/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_QUICK_QUICKPEN_H
#define KDSME_QUICK_QUICKPEN_H

#include <QColor>
#include <QObject>

class QPen;

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
    explicit QuickPen(QObject* parent = nullptr);

    qreal width() const;
    void setWidth(qreal width);

    QColor color() const;
    void setColor(const QColor& color);

    Qt::PenStyle style() const;
    void setStyle(Qt::PenStyle style);

    QPen toQPen() const;
    void modifyPen(QPen* pen) const;

Q_SIGNALS:
    void penChanged();

private:
    qreal m_width;
    QColor m_color;
    Qt::PenStyle m_style;
};

#endif // QUICKPEN_H
