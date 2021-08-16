#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
if(NOT BUILD_QT6)
    qt5_generate_repc(REPC_SRCS ../../src/debuginterface/debuginterface.rep REPLICA)

    qt5_add_statecharts(STATECHART_SRCS statemachine.scxml)
    # Work-around QTBUG-84387
    # Needed in Qt5, otherwise we'll run into duplicate symbol issues like this:
    #   ld: error: duplicate symbol: TrafficLightStateMachine::qt_metacall(QMetaObject::Call, int, void**)
    set_source_files_properties(${STATECHART_SRCS} PROPERTIES
                                                GENERATED TRUE
                                                SKIP_AUTOGEN ON)
    target_sources(qscxmldebugger PRIVATE ${REPC_SRCS} ${STATECHART_SRCS})
endif()
if(BUILD_QT6)
    qt6_add_repc_replicas(qscxmldebugger ../../src/debuginterface/debuginterface.rep)
    qt6_add_statecharts(qscxmldebugger statemachine.scxml)
endif()
target_link_libraries(
    qscxmldebugger
    KDSME::Core
    KDSME::View
    KDSME::DebugInterfaceClient
    KDSME::DebugInterfaceSource
    Qt${Qt_VERSION_MAJOR}::Widgets
    Qt${Qt_VERSION_MAJOR}::RemoteObjects
    Qt${Qt_VERSION_MAJOR}::Scxml
)
