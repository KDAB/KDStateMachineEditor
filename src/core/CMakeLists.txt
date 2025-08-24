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

set(LIB_SRCS
    debug.cpp
    debug.h
    export/abstractexporter.cpp
    export/abstractexporter.h
    export/qmlexporter.cpp
    export/qmlexporter.h
    export/scxmlexporter.cpp
    export/scxmlexporter.h
    export/svgexporter.cpp
    export/svgexporter.h
    import/abstractimporter.cpp
    import/abstractimporter.h
    import/scxmlimporter.cpp
    import/scxmlimporter.h
    layout/layerwiselayouter.cpp
    layout/layerwiselayouter.h
    layout/layouter.cpp
    layout/layouter.h
    layout/layoutimportexport.cpp
    layout/layoutimportexport.h
    layout/layoutproperties.cpp
    layout/layoutproperties.h
    layout/layoututils.cpp
    layout/layoututils.h
    model/element.cpp
    model/element.h
    model/elementfactory.cpp
    model/elementfactory.h
    model/elementmodel.cpp
    model/elementmodel.h
    model/elementutil.cpp
    model/elementutil.h
    model/runtimecontroller.cpp
    model/runtimecontroller.h
    model/state.cpp
    model/state.h
    model/transition.cpp
    model/transition.h
    util/depthchecker.cpp
    util/depthchecker.h
    util/objecthelper.cpp
    util/objecthelper.h
    util/objecttreemodel.cpp
    util/objecttreemodel.h
)

if(GRAPHVIZ_FOUND)
    list(
        APPEND
        LIB_SRCS
        layout/graphvizlayout/graphvizlayerlayouter.cpp
        layout/graphvizlayout/graphvizlayerlayouter.h
        layout/graphvizlayout/graphvizlayouter.cpp
        layout/graphvizlayout/graphvizlayouter.h
        layout/graphvizlayout/graphvizlayouterbackend.cpp
        layout/graphvizlayout/graphvizlayouterbackend_p.h
        layout/graphvizlayout/gvutils.cpp
        layout/graphvizlayout/gvutils.h
    )
    if(KDSME_STATIC_GRAPHVIZ)
        list(APPEND LIB_SRCS layout/graphvizlayout/graphviz_staticbuild.c)
    endif()
endif()

add_library(kdstatemachineeditor_core ${LIB_SRCS})
add_library(KDSME::Core ALIAS kdstatemachineeditor_core)
target_link_libraries(
    kdstatemachineeditor_core
    PRIVATE Qt${QT_VERSION_MAJOR}::Quick # TODO: Get rid off this
            Qt6::StateMachine
    PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui
)

if(NOT KDSME_INTERNAL_GRAPHVIZ)
    target_include_directories(kdstatemachineeditor_core PRIVATE ${GRAPHVIZ_INCLUDE_DIR})
endif()

set_target_properties(
    kdstatemachineeditor_core
    PROPERTIES SOVERSION ${KDSME_SOVERSION}
               VERSION ${KDSME_SOVERSION}
               EXPORT_NAME Core
               INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)
#version libraries on Windows
if(WIN32)
    set(postfix ${KDSME_SOVERSION})
    string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_BUILD_TYPE)
    if(${UPPER_BUILD_TYPE} MATCHES "^DEBUG")
        string(CONCAT postfix ${postfix} "d")
        set_target_properties(kdstatemachineeditor_core PROPERTIES DEBUG_POSTFIX ${postfix})
    else()
        set_target_properties(kdstatemachineeditor_core PROPERTIES ${UPPER_BUILD_TYPE}_POSTFIX ${postfix})
    endif()
endif()

generate_export_header(kdstatemachineeditor_core EXPORT_FILE_NAME kdsme_core_export.h BASE_NAME KDSME_CORE)

if(KDSME_INTERNAL_GRAPHVIZ)
    set(GRAPHVIZ_GVC_LIBRARY gvc)
    set(GRAPHVIZ_CGRAPH_LIBRARY cgraph)
    if(NOT GRAPHVIZ_PLUGIN_DOT_LAYOUT_LIBRARY)
        set(GRAPHVIZ_PLUGIN_DOT_LAYOUT_LIBRARY gvplugin_dot_layout)
    endif()
endif()

if(GRAPHVIZ_FOUND)
    target_link_libraries(
        kdstatemachineeditor_core PRIVATE ${GRAPHVIZ_CGRAPH_LIBRARY} ${GRAPHVIZ_GVC_LIBRARY}
                                          ${GRAPHVIZ_PLUGIN_DOT_LAYOUT_LIBRARY}
    )
endif()

set(build_iface_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/common ${CMAKE_CURRENT_SOURCE_DIR}/export ${CMAKE_CURRENT_SOURCE_DIR}/import
    ${CMAKE_CURRENT_SOURCE_DIR}/layout ${CMAKE_CURRENT_SOURCE_DIR}/model ${CMAKE_CURRENT_SOURCE_DIR}/util
)
target_include_directories(
    kdstatemachineeditor_core PUBLIC "$<BUILD_INTERFACE:${build_iface_dirs}>"
                                     $<INSTALL_INTERFACE:${INCLUDE_INSTALL_ROOT}>
)

set_target_properties(kdstatemachineeditor_core PROPERTIES OUTPUT_NAME "kdstatemachineeditor_core${KDSME_LIBRARY_QTID}")

if(KDSME_INTERNAL_GRAPHVIZ)
    install(
        TARGETS gvc cgraph cdt gvplugin_dot_layout dotgen util
        EXPORT KDSME_TARGETS
        ${INSTALL_TARGETS_DEFAULT_ARGS}
    )
    if(KDSME_STATIC_GRAPHVIZ)
        install(
            TARGETS common pack ortho pathplan label xdot
            EXPORT KDSME_TARGETS
            ${INSTALL_TARGETS_DEFAULT_ARGS}
        )
    endif()
endif()
install(
    TARGETS kdstatemachineeditor_core
    EXPORT KDSME_TARGETS
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(MSVC AND BUILD_SHARED_LIBS)
    install(
        FILES "$<TARGET_PDB_FILE_DIR:kdstatemachineeditor_core>/$<TARGET_PDB_FILE_NAME:kdstatemachineeditor_core>"
        DESTINATION ${BIN_INSTALL_DIR}
        CONFIGURATIONS Debug RelWithDebInfo
    )
endif()
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/kdsme_core_export.h
          export/abstractexporter.h
          export/qmlexporter.h
          export/scxmlexporter.h
          import/abstractimporter.h
          import/scxmlimporter.h
          model/element.h
          model/elementmodel.h
          model/elementutil.h
          model/runtimecontroller.h
          model/state.h
          model/transition.h
          layout/layouter.h
          layout/layoutimportexport.h
          layout/layoutproperties.h
          util/objecthelper.h
          util/objecttreemodel.h
          util/ringbuffer.h
          util/treewalker.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/core
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
