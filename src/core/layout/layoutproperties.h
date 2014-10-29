/*
  layoutproperties.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_LAYOUT_LAYOUTPROPERTIES_H
#define KDSME_LAYOUT_LAYOUTPROPERTIES_H

#include "kdsme_core_export.h"

#include <QFont>
#include <QObject>
#include <QRectF>
#include <QVariant>

namespace KDSME {

class KDSME_CORE_EXPORT LayoutProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal regionMargins READ regionMargins WRITE setRegionMargins NOTIFY regionMarginsChanged FINAL)
    Q_PROPERTY(QFont regionLabelFont READ regionLabelFont WRITE setRegionLabelFont NOTIFY regionLabelFontChanged FINAL)
    Q_PROPERTY(qreal regionLabelMargins READ regionLabelMargins WRITE setRegionLabelMargins NOTIFY regionLabelMarginsChanged FINAL)
    Q_PROPERTY(QSizeF regionLabelButtonBoxSize READ regionLabelButtonBoxSize WRITE setRegionLabelButtonBoxSize NOTIFY regionLabelButtonBoxSizeChanged FINAL)

public:
    explicit LayoutProperties(QObject* parent = nullptr);
    virtual ~LayoutProperties();

    qreal regionMargins() const;
    void setRegionMargins(qreal margins);

    QFont regionLabelFont() const;
    void setRegionLabelFont(const QFont& font);

    qreal regionLabelMargins() const;
    void setRegionLabelMargins(qreal margins);

    QSizeF regionLabelButtonBoxSize() const;
    void setRegionLabelButtonBoxSize(const QSizeF& size);

Q_SIGNALS:
    void regionMarginsChanged(qreal);
    void regionLabelFontChanged(const QFont&);
    void regionLabelMarginsChanged(qreal);
    void regionLabelButtonBoxSizeChanged(const QSizeF&);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // LAYOUTPROPERTIES_H
