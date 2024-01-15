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

#ifndef KDSME_VIEW_INSTANTIATORINTERFACE_P_H
#define KDSME_VIEW_INSTANTIATORINTERFACE_P_H

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QObject;
QT_END_NAMESPACE

class InstantiatorInterface
{
public:
    virtual ~InstantiatorInterface()
    {
    }

    virtual QObject *itemForIndex(const QModelIndex &index) const = 0;
};

#endif
