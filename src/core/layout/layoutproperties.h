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
    Q_PROPERTY(bool showTransitionLabels READ showTransitionLabels WRITE setShowTransitionLabels NOTIFY showTransitionLabelsChanged FINAL)

public:
    explicit LayoutProperties(QObject *parent = nullptr);
    ~LayoutProperties();

    qreal regionMargins() const;
    void setRegionMargins(qreal margins);

    QFont regionLabelFont() const;
    void setRegionLabelFont(const QFont &font);

    qreal regionLabelMargins() const;
    void setRegionLabelMargins(qreal margins);

    QSizeF regionLabelButtonBoxSize() const;
    void setRegionLabelButtonBoxSize(const QSizeF &size);

    bool showTransitionLabels() const;
    void setShowTransitionLabels(bool show);

Q_SIGNALS:
    void regionMarginsChanged(qreal);
    void regionLabelFontChanged(const QFont &);
    void regionLabelMarginsChanged(qreal);
    void regionLabelButtonBoxSizeChanged(const QSizeF &);
    void showTransitionLabelsChanged(bool);
    void changed();

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // LAYOUTPROPERTIES_H
