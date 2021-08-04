#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-examples.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-examples.h
)

add_subdirectory(layoutexport)
add_subdirectory(export)
if(Qt${Qt_VERSION_MAJOR}RemoteObjects_FOUND)
    add_subdirectory(qsmdebugger)
    if(Qt${Qt_VERSION_MAJOR}Scxml_FOUND)
        add_subdirectory(qscxmldebugger)
    endif()
endif()
add_subdirectory(scxmlimport)
add_subdirectory(simplestatemachine)
