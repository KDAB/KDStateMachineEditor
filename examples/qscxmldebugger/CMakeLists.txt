# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

add_executable(qscxmldebugger trafficlight.cpp main.cpp trafficlight-widgets-static.qrc)

# Copy the .rep file to your project
qt_add_repc_replicas(qscxmldebugger ../../src/debuginterface/debuginterface.rep)
qt_add_statecharts(qscxmldebugger statemachine.scxml)

target_link_libraries(
    qscxmldebugger
    KDSME::Core
    KDSME::View
    KDSME::DebugInterfaceClient
    KDSME::DebugInterfaceSource
    Qt::Widgets
    Qt::RemoteObjects
    Qt::Scxml
)
