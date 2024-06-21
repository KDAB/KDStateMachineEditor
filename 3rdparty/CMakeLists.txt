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

        if(WITH_STATIC_GRAPHVIZ)
            set(CMAKE_POSITION_INDEPENDENT_CODE ON
            )#TODO remove after https://gitlab.com/graphviz/graphviz/-/merge_requests/3823 is merged
            set(BUILD_SHARED_LIBS OFF)
        endif()
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/graphviz/CMakeLists.txt")
            message(FATAL_ERROR "Please do git submodule update --init --recursive")
        endif()

        # cmake-lint: disable=C0103
        set(with_gvedit
            FALSE
            CACHE BOOL " GVEdit interactive graph editor" FORCE
        )
        add_subdirectory(graphviz)

        if(WIN32)
            # On Windows we need some dependencies available for tests
            file(GLOB GRAPHVIZ_DEPENDENCIES
                 ${CMAKE_CURRENT_SOURCE_DIR}/graphviz/windows/dependencies/libraries/x64/bin/*
            )
            file(COPY ${GRAPHVIZ_DEPENDENCIES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        endif()
    endfunction()

    add_graphviz_subdirectory()
endif()
