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

#ifndef PARSEHELPER
#define PARSEHELPER

#include <qglobal.h>

namespace KDSME {
class StateMachine;
}

QT_BEGIN_NAMESPACE
class QByteArray;
class QString;
QT_END_NAMESPACE

#include <QtCore/qglobal.h>

namespace ParseHelper {
QByteArray readFile(const QString &fileName);
}

#endif
