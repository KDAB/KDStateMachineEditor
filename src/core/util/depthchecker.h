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

#ifndef KDSME_DEPTHCHECKER_H
#define KDSME_DEPTHCHECKER_H

#include "kdsme_core_export.h"

#include "element.h"

#include <QObject>

namespace KDSME {

class KDSME_CORE_EXPORT DepthChecker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KDSME::Element *target READ target WRITE setTarget NOTIFY targetChanged FINAL)
    Q_PROPERTY(int depth READ depth NOTIFY depthChanged FINAL)

public:
    explicit DepthChecker(QObject *parent = nullptr);
    ~DepthChecker();

    Element *target() const;
    void setTarget(Element *target);

    int depth() const;

Q_SIGNALS:
    void targetChanged(QObject *target);
    void depthChanged(int depth);

private:
    struct Private;
    QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void updateDepth())
};

}

#endif // KDSME_DEPTHCHECKER_H
