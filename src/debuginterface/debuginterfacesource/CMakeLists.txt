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

set(QSMDEBUGINTERFACESOURCE_SRCS qsmdebuginterfacesource.cpp qsmwatcher.cpp ../../core/util/objecthelper.cpp)

if(Qt${QT_VERSION_MAJOR}Scxml_FOUND)
    list(APPEND QSMDEBUGINTERFACESOURCE_SRCS qscxmldebuginterfacesource.cpp)
endif()

add_library(kdstatemachineeditor_debuginterfacesource STATIC ${QSMDEBUGINTERFACESOURCE_SRCS})

qt6_add_repc_sources(kdstatemachineeditor_debuginterfacesource ../debuginterface.rep)
target_link_libraries(kdstatemachineeditor_debuginterfacesource LINK_PRIVATE Qt6::StateMachine)

add_library(KDSME::DebugInterfaceSource ALIAS kdstatemachineeditor_debuginterfacesource)
target_link_libraries(
    kdstatemachineeditor_debuginterfacesource LINK_PRIVATE Qt${QT_VERSION_MAJOR}::RemoteObjects
    kdstatemachineeditor_core LINK_PUBLIC Qt${QT_VERSION_MAJOR}::Core
)
if(Qt${QT_VERSION_MAJOR}Scxml_FOUND)
    target_link_libraries(
        kdstatemachineeditor_debuginterfacesource LINK_PRIVATE Qt${QT_VERSION_MAJOR}::ScxmlPrivate LINK_PUBLIC
        Qt${QT_VERSION_MAJOR}::Scxml
    )
endif()
set_target_properties(
    kdstatemachineeditor_debuginterfacesource
    PROPERTIES SOVERSION ${KDSME_SOVERSION}
               VERSION ${KDSME_SOVERSION}
               EXPORT_NAME DebugInterfaceSource
               INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)
#version libraries on Windows
if(WIN32)
    set(postfix ${KDSME_SOVERSION})
    string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_BUILD_TYPE)
    if(${UPPER_BUILD_TYPE} MATCHES "^DEBUG")
        string(CONCAT postfix ${postfix} "d")
        set_target_properties(kdstatemachineeditor_debuginterfacesource PROPERTIES DEBUG_POSTFIX ${postfix})
    else()
        set_target_properties(
            kdstatemachineeditor_debuginterfacesource PROPERTIES ${UPPER_BUILD_TYPE}_POSTFIX ${postfix}
        )
    endif()
endif()

target_compile_definitions(kdstatemachineeditor_debuginterfacesource PRIVATE -Dkdstatemachineeditor_core_EXPORTS)
generate_export_header(
    kdstatemachineeditor_debuginterfacesource EXPORT_FILE_NAME kdsme_debuginterfacesource_export.h BASE_NAME
    KDSME_DEBUGINTERFACESOURCE
)
target_include_directories(
    kdstatemachineeditor_debuginterfacesource
    PUBLIC $<INSTALL_INTERFACE:${INCLUDE_INSTALL_ROOT}>
    PRIVATE ../
)

set_target_properties(
    kdstatemachineeditor_debuginterfacesource
    PROPERTIES OUTPUT_NAME "kdstatemachineeditor_debuginterfacesource${KDSME_LIBRARY_QTID}"
)
install(
    TARGETS kdstatemachineeditor_debuginterfacesource
    EXPORT KDSME_TARGETS
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)
# No need to install PDB, since there's no PDB generated, it's a static library.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kdsme_debuginterfacesource_export.h qsmdebuginterfacesource.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/debuginterfacesource
)
