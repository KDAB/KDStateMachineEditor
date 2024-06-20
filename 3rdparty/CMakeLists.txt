#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2024 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Daniel Nicoletti <daniel.nicoletti@kdab.com>
#
# SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor
#
# Licensees holding valid commercial KDAB State Machine Editor Library
# licenses may use this file in accordance with the KDAB State Machine Editor
# Library License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

if(WITH_INTERNAL_GRAPHVIZ)
    # Function creates extra scope to keep these variables local
    function(add_graphviz_subdirectory)
        #TODO check if there are unneeded features to disable

        set(with_gvedit OFF)
        if(WITH_STATIC_GRAPHVIZ)
            set(CMAKE_POSITION_INDEPENDENT_CODE ON)
            set(BUILD_SHARED_LIBS OFF)
        endif()
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/graphviz/CMakeLists.txt")
            message(FATAL_ERROR "Please do git submodule update --init --recursive")
        endif()

        add_subdirectory(graphviz)
    endfunction()

    add_graphviz_subdirectory()
endif()
