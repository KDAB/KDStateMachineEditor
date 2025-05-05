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

#ifndef TESTS_UTIL_H
#define TESTS_UTIL_H

#include <qglobal.h>
#include <chrono>

QT_BEGIN_NAMESPACE
class QObject;
QT_END_NAMESPACE

namespace TestUtil {

/**
 * Starts an event loop that runs until the given signal is received.
 * Optionally the event loop
 * can return earlier on a timeout.
 *
 * \return \p true if the requested signal was received
 *         \p false on timeout
 */
bool waitForSignal(const QObject *obj, const char *signal, std::chrono::milliseconds timeout = std::chrono::seconds { 1 });

}

#endif
