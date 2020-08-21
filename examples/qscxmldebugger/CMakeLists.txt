#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
# Copy the .rep file to your project
qt5_generate_repc(SRCS ../../src/debuginterface/debuginterface.rep REPLICA)
qt5_add_resources(SRCS trafficlight-widgets-static.qrc)
qt5_add_statecharts(SRCS statemachine.scxml)
add_executable(qscxmldebugger ${SRCS})
target_link_libraries(qscxmldebugger
  KDSME::Core
  KDSME::View
  KDSME::DebugInterfaceClient
  KDSME::DebugInterfaceSource
  Qt5::Widgets
  Qt5::RemoteObjects
  Qt5::Scxml
)
