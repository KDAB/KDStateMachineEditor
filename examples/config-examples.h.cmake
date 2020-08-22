/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef CONFIG_EXAMPLES_H
#define CONFIG_EXAMPLES_H

#include <qglobal.h>

#cmakedefine TEST_DATA_DIR "${TEST_DATA_DIR}"

#cmakedefine Qt5RemoteObjects_VERSION QT_VERSION_CHECK(${Qt5RemoteObjects_VERSION_MAJOR}, ${Qt5RemoteObjects_VERSION_MINOR}, ${Qt5RemoteObjects_VERSION_PATCH})

#endif
