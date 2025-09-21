# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

set(SRCS trafficlight.cpp main.cpp)

add_executable(qsmdebugger ${SRCS})

qt6_add_repc_replicas(qsmdebugger ../../src/debuginterface/debuginterface.rep)
target_link_libraries(qsmdebugger Qt6::StateMachine)

target_link_libraries(
    qsmdebugger
    KDSME::Core
    KDSME::View
    KDSME::DebugInterfaceClient
    KDSME::DebugInterfaceSource
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::RemoteObjects
)
