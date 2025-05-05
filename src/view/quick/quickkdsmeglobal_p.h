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

#ifndef KDSME_QUICK_QUICKKDSMEGLOBAL_P_H
#define KDSME_QUICK_QUICKKDSMEGLOBAL_P_H

#include <QObject>

class QuickPainterPath;

class QuickKDSMEGlobal : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public:
    Q_INVOKABLE QuickPainterPath *createPath() const;
    Q_INVOKABLE QuickPainterPath *createPath(QPointF startPoint) const;
};

#endif // QUICKKDSMEGLOBAL_P_H
