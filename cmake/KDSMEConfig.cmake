# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Kevin Funk <kevin.funk@kdab.com>
#
# SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor
#
# Licensees holding valid commercial KDAB State Machine Editor Library
# licenses may use this file in accordance with the KDAB State Machine Editor
# Library License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

include(CMakeFindDependencyMacro)

find_dependency(Qt@QT_VERSION_MAJOR@Widgets)
find_dependency(Qt@QT_VERSION_MAJOR@Quick)
find_dependency(Qt@QT_VERSION_MAJOR@QuickWidgets)

include("${CMAKE_CURRENT_LIST_DIR}/KDSME@KDSME_LIBRARY_QTID@Targets.cmake")
