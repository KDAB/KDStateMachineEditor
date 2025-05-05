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

if(KDSME_INTERNAL_GRAPHVIZ)
    # Function creates extra scope to keep these variables local
    function(add_graphviz_subdirectory)
        #TODO check if there are unneeded features to disable

        if(KDSME_STATIC_GRAPHVIZ)
            set(BUILD_SHARED_LIBS OFF)
        endif()
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/graphviz/CMakeLists.txt")
            message(FATAL_ERROR "Please do git submodule update --init --recursive")
        endif()

        # Disable all command line tools
        set(GRAPHVIZ_CLI OFF)
        set(CMAKE_AUTOMOC OFF)

        set(ENABLE_TCL
            OFF
            CACHE STRING "Build TCL components" FORCE
        )
        set(ENABLE_SWIG
            OFF
            CACHE STRING "Build language bindings" FORCE
        )
        set(ENABLE_GO
            OFF
            CACHE STRING "Build Go components" FORCE
        )
        set(WITH_EXPAT
            OFF
            CACHE STRING "Support HTML-like labels through expat" FORCE
        )

        # cmake-lint: disable=C0301
        if(WIN32)
            # On Windows we need some dependencies available for tests
            file(GLOB GRAPHVIZ_DEPENDENCIES
                 ${CMAKE_CURRENT_SOURCE_DIR}/graphviz/windows/dependencies/libraries/x64/bin/*
            )
            file(COPY ${GRAPHVIZ_DEPENDENCIES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

            list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/graphviz/windows/dependencies/libraries)
            list(APPEND CMAKE_PROGRAM_PATH
                 ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/graphviz/windows/dependencies/graphviz-build-utilities
            )
            set(BISON_EXECUTABLE
                ${CMAKE_CURRENT_SOURCE_DIR}/graphviz/windows/dependencies/graphviz-build-utilities/winflexbison/win_bison.exe
            )
            set(FLEX_EXECUTABLE
                ${CMAKE_CURRENT_SOURCE_DIR}/graphviz/windows/dependencies/graphviz-build-utilities/winflexbison/win_flex.exe
            )
        endif()

        add_subdirectory(graphviz)
    endfunction()

    add_graphviz_subdirectory()
endif()
