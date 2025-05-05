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

#include "parsehelper.h"
#include "debug.h"
#include "scxmlimporter.h"
#include "element.h"

#include <config-test.h>

#include <QFile>

using namespace KDSME;

QByteArray ParseHelper::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        qWarning() << "File does not exist:" << file.fileName();
        return QByteArray();
    }
    const bool success = file.open(QIODevice::ReadOnly);
    if (!success)
        return QByteArray();

    const QByteArray data = file.readAll();
    return data;
}
