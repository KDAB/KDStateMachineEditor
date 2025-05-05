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

#include "util.h"

#include <QEventLoop>
#include <QObject>
#include <QSignalSpy>
#include <QTimer>

bool TestUtil::waitForSignal(const QObject *obj, const char *signal, std::chrono::milliseconds timeout)
{
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit())); // clazy:exclude=old-style-connect
    QTimer timer;
    const QSignalSpy timeoutSpy(&timer, &QTimer::timeout);
    if (timeout.count() > 0) {
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}
