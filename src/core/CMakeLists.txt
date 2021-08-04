#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
  export/abstractexporter.cpp
  export/qmlexporter.cpp
  export/scxmlexporter.cpp
  export/svgexporter.cpp

  layout/layerwiselayouter.cpp
  layout/layouter.cpp
  layout/layoutimportexport.cpp
  layout/layoutproperties.cpp
  layout/layoututils.cpp

  model/element.cpp
  model/elementfactory.cpp
  model/elementmodel.cpp
  model/elementutil.cpp
  model/runtimecontroller.cpp
  model/state.cpp
  model/transition.cpp

  import/abstractimporter.cpp
  import/scxmlimporter.cpp

  util/depthchecker.cpp
  util/objecthelper.cpp
  util/objecttreemodel.cpp

  debug.cpp
)

if(GRAPHVIZ_FOUND)
  list(APPEND LIB_SRCS
    layout/graphvizlayout/gvutils.cpp
    layout/graphvizlayout/graphvizlayerlayouter.cpp
    layout/graphvizlayout/graphvizlayouter.cpp
    layout/graphvizlayout/graphvizlayouterbackend.cpp
  )
  if(WITH_STATIC_GRAPHVIZ)
    list(APPEND LIB_SRCS
      layout/graphvizlayout/graphviz_staticbuild.c
    )
  endif()
  add_definitions(${GRAPHVIZ_COMPILE_FLAGS})
endif()

add_library(kdstatemachineeditor_core SHARED ${LIB_SRCS})
add_library(KDSME::Core ALIAS kdstatemachineeditor_core)
target_link_libraries(kdstatemachineeditor_core
  LINK_PRIVATE
    Qt${Qt_VERSION_MAJOR}::Quick # TODO: Get rid off this
  LINK_PUBLIC
    Qt${Qt_VERSION_MAJOR}::Core
    Qt${Qt_VERSION_MAJOR}::Gui
)

if (BUILD_QT6)
  target_link_libraries(kdstatemachineeditor_core
    LINK_PRIVATE
      Qt6::StateMachine
  )
endif()

set_target_properties(kdstatemachineeditor_core PROPERTIES
  SOVERSION ${KDSME_SOVERSION}
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

if(GRAPHVIZ_FOUND)
  if(WITH_STATIC_GRAPHVIZ)
    target_link_libraries(kdstatemachineeditor_core LINK_PRIVATE GraphvizStaticDotLayoutPlugin)
  endif()
  target_link_libraries(kdstatemachineeditor_core LINK_PRIVATE ${GRAPHVIZ_GVC_LIBRARY} ${GRAPHVIZ_CGRAPH_LIBRARY})
  if(WITH_INTERNAL_GRAPHVIZ)
      add_dependencies(kdstatemachineeditor_core GraphvizExternalProject)
  endif()
  target_include_directories(kdstatemachineeditor_core PRIVATE ${GRAPHVIZ_INCLUDE_DIR})
endif()

set(build_iface_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}/common
  ${CMAKE_CURRENT_SOURCE_DIR}/export
  ${CMAKE_CURRENT_SOURCE_DIR}/import
  ${CMAKE_CURRENT_SOURCE_DIR}/layout
  ${CMAKE_CURRENT_SOURCE_DIR}/model
  ${CMAKE_CURRENT_SOURCE_DIR}/util
)
target_include_directories(kdstatemachineeditor_core
  PUBLIC
    "$<BUILD_INTERFACE:${build_iface_dirs}>"
    # FIXME: It is recommended to use a relative path here.
    # Doesn't work with CMake 2.8.11, though, probably needs 2.8.12(?)
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)

set_target_properties(kdstatemachineeditor_core PROPERTIES OUTPUT_NAME "kdstatemachineeditor_core${KDSME_LIBRARY_QTID}")

install(TARGETS kdstatemachineeditor_core EXPORT KDSME_TARGETS ${INSTALL_TARGETS_DEFAULT_ARGS})
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:kdstatemachineeditor_core>/$<TARGET_PDB_FILE_NAME:kdstatemachineeditor_core>" DESTINATION ${LIB_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kdsme_core_export.h

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

if (NOT BUILD_QT6)
  ecm_generate_pri_file(BASE_NAME KDSMECore
                        LIB_NAME kdstatemachineeditor_core
                        DEPS "core gui"
                        FILENAME_VAR PRI_FILENAME
                        INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/core
  )
  install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
endif()

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
