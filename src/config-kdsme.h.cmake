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

#ifndef KDSME_CONFIG_H
#define KDSME_CONFIG_H

#include <qglobal.h>

/* Define to the full name of this package. */
#define PACKAGE_NAME "${CMAKE_PROJECT_NAME}"

#cmakedefine01 HAVE_GRAPHVIZ

#cmakedefine01 KDSME_STATIC_GRAPHVIZ

#endif
