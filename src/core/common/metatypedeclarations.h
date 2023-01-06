/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_COMMON_METATYPEDECLARATIONS_H
#define KDSME_COMMON_METATYPEDECLARATIONS_H

#include <QMetaType>
#include <QPainterPath>

Q_DECLARE_METATYPE(QPainterPath)
Q_DECLARE_METATYPE(Qt::PenStyle)

Q_DECLARE_METATYPE(QUndoCommand *)

#endif
