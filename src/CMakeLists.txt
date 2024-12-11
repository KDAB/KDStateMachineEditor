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

add_definitions(
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
    -DQT_USE_FAST_OPERATOR_PLUS
)

set(HAVE_GRAPHVIZ ${GRAPHVIZ_FOUND})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kdsme.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kdsme.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)
add_subdirectory(view)

if(TARGET Qt${QT_VERSION_MAJOR}::RemoteObjects)
    add_subdirectory(debuginterface)
endif()

if(BUILD_TESTING)
    add_subdirectory(app)
endif()
