/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_LAYOUT_LAYOUTER_H
#define KDSME_LAYOUT_LAYOUTER_H

#include "kdsme_core_export.h"

#include <QObject>
#include <QList>

QT_BEGIN_NAMESPACE
class QRectF;
QT_END_NAMESPACE

namespace KDSME {

class LayoutProperties;
class State;

class KDSME_CORE_EXPORT Layouter : public QObject
{
    Q_OBJECT

public:
    explicit Layouter(QObject* parent = nullptr);

    virtual QRectF layout(State* state, const LayoutProperties* properties) = 0;
};

}

#endif
