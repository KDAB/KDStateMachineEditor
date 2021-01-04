#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

include("${CMAKE_CURRENT_LIST_DIR}/KDSMEFindDependencyMacro.cmake")

kdsme_find_dependency(Qt5Widgets)
kdsme_find_dependency(Qt5Quick)
kdsme_find_dependency(Qt5QuickWidgets)

include("${CMAKE_CURRENT_LIST_DIR}/KDSMETargets.cmake")
