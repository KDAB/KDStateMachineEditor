#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

set(HAVE_GRAPHVIZ ${GRAPHVIZ_FOUND})
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-kdsme.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-kdsme.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)
add_subdirectory(view)

if(Qt5RemoteObjects_FOUND)
  add_subdirectory(debuginterface)
endif()

if(BUILD_TESTS)
  add_subdirectory(app)
endif()
