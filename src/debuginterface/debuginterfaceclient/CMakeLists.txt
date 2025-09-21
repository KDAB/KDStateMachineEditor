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

set(DEBUGINTERFACECLIENT_SRCS debuginterfaceclient.cpp)

add_library(kdstatemachineeditor_debuginterfaceclient ${DEBUGINTERFACECLIENT_SRCS})

qt6_add_repc_replicas(kdstatemachineeditor_debuginterfaceclient ../debuginterface.rep)

add_library(KDSME::DebugInterfaceClient ALIAS kdstatemachineeditor_debuginterfaceclient)
target_link_libraries(
    kdstatemachineeditor_debuginterfaceclient LINK_PRIVATE Qt${QT_VERSION_MAJOR}::RemoteObjects LINK_PUBLIC KDSME::Core
    Qt${QT_VERSION_MAJOR}::Core
)
set_target_properties(
    kdstatemachineeditor_debuginterfaceclient
    PROPERTIES SOVERSION ${KDSME_SOVERSION}
               VERSION ${KDSME_SOVERSION}
               EXPORT_NAME DebugInterfaceClient
               INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)
#version libraries on Windows
if(WIN32)
    set(postfix ${KDSME_SOVERSION})
    string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_BUILD_TYPE)
    if(${UPPER_BUILD_TYPE} MATCHES "^DEBUG")
        string(CONCAT postfix ${postfix} "d")
        set_target_properties(kdstatemachineeditor_debuginterfaceclient PROPERTIES DEBUG_POSTFIX ${postfix})
    else()
        set_target_properties(
            kdstatemachineeditor_debuginterfaceclient PROPERTIES ${UPPER_BUILD_TYPE}_POSTFIX ${postfix}
        )
    endif()
endif()

generate_export_header(
    kdstatemachineeditor_debuginterfaceclient EXPORT_FILE_NAME kdsme_debuginterfaceclient_export.h BASE_NAME
    KDSME_DEBUGINTERFACECLIENT
)
set(build_iface_dirs ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_include_directories(
    kdstatemachineeditor_debuginterfaceclient PUBLIC "$<BUILD_INTERFACE:${build_iface_dirs}>"
                                                     $<INSTALL_INTERFACE:${INCLUDE_INSTALL_ROOT}>
)

set_target_properties(
    kdstatemachineeditor_debuginterfaceclient
    PROPERTIES OUTPUT_NAME "kdstatemachineeditor_debuginterfaceclient${KDSME_LIBRARY_QTID}"
)
install(
    TARGETS kdstatemachineeditor_debuginterfaceclient
    EXPORT KDSME_TARGETS
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(MSVC AND BUILD_SHARED_LIBS)
    # cmake-lint: disable=C0301
    install(
        FILES
            "$<TARGET_PDB_FILE_DIR:kdstatemachineeditor_debuginterfaceclient>/$<TARGET_PDB_FILE_NAME:kdstatemachineeditor_debuginterfaceclient>"
        DESTINATION ${BIN_INSTALL_DIR}
        CONFIGURATIONS Debug RelWithDebInfo
    )
endif()
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kdsme_debuginterfaceclient_export.h debuginterfaceclient.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/debuginterfaceclient
)
