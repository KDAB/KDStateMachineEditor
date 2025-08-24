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

# Make the files show up in Qt Creator
file(GLOB_RECURSE QML_JS_FILES *.qml *.js)

add_library(
    kdstatemachineeditor_view
    command/command.cpp
    command/commandfactory.cpp
    command/createelementcommand.cpp
    command/deleteelementcommand.cpp
    command/layoutsnapshotcommand.cpp
    command/modifyinitialstatecommand.cpp
    command/modifydefaultstatecommand.cpp
    command/modifyelementcommand.cpp
    command/modifypropertycommand.cpp
    command/modifytransitioncommand.cpp
    command/reparentelementcommand.cpp
    command/changestatemachinecommand.cpp
    quick/qopengl2pexvertexarray.cpp # Copy because QtGui doesn't export all symbols
    quick/quickmaskedmousearea.cpp
    quick/quickpainterpath.cpp
    quick/quickpen.cpp
    quick/quickprimitiveitem.cpp
    quick/quickkdsmeglobal.cpp
    quick/quickrecursiveinstantiator.cpp
    quick/quicksceneitem.cpp
    kdsme.qrc
    widgets/codeeditor.cpp
    widgets/propertyeditor.cpp
    widgets/statemachinetoolbar.cpp
    widgets/statemachineview.cpp
    widgets/statemachinepalettewidget.cpp
    widgets/statepropertyeditor.ui
    widgets/transitionpropertyeditor.ui
    abstractscene.cpp
    statemachinescene.cpp
    abstractcontroller.cpp
    commandcontroller.cpp
    debug.cpp
    editcontroller.cpp
    semanticzoommanager.cpp
    ${QML_JS_FILES}
)

add_library(KDSME::View ALIAS kdstatemachineeditor_view)
target_link_libraries(
    kdstatemachineeditor_view
    PRIVATE Qt${QT_VERSION_MAJOR}::GuiPrivate
    PUBLIC Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::Widgets
           kdstatemachineeditor_core
)

set(build_iface_dirs ${CMAKE_CURRENT_SOURCE_DIR}/command ${CMAKE_CURRENT_SOURCE_DIR}/widgets)

set_target_properties(
    kdstatemachineeditor_view
    PROPERTIES SOVERSION ${KDSME_SOVERSION}
               VERSION ${KDSME_SOVERSION}
               EXPORT_NAME View
               INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)
#version libraries on Windows
if(WIN32)
    set(postfix ${KDSME_SOVERSION})
    string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_BUILD_TYPE)
    if(${UPPER_BUILD_TYPE} MATCHES "^DEBUG")
        string(CONCAT postfix ${postfix} "d")
        set_target_properties(kdstatemachineeditor_view PROPERTIES DEBUG_POSTFIX ${postfix})
    else()
        set_target_properties(kdstatemachineeditor_view PROPERTIES ${UPPER_BUILD_TYPE}_POSTFIX ${postfix})
    endif()
endif()

generate_export_header(kdstatemachineeditor_view EXPORT_FILE_NAME kdsme_view_export.h BASE_NAME KDSME_VIEW)
target_include_directories(
    kdstatemachineeditor_view PUBLIC "$<BUILD_INTERFACE:${build_iface_dirs}>"
                                     $<INSTALL_INTERFACE:${INCLUDE_INSTALL_ROOT}>
)

set_target_properties(kdstatemachineeditor_view PROPERTIES OUTPUT_NAME "kdstatemachineeditor_view${KDSME_LIBRARY_QTID}")

install(
    TARGETS kdstatemachineeditor_view
    EXPORT KDSME_TARGETS
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)
if(MSVC AND BUILD_SHARED_LIBS)
    install(
        FILES "$<TARGET_PDB_FILE_DIR:kdstatemachineeditor_view>/$<TARGET_PDB_FILE_NAME:kdstatemachineeditor_view>"
        DESTINATION ${BIN_INSTALL_DIR}
        CONFIGURATIONS Debug RelWithDebInfo
    )
endif()
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/kdsme_view_export.h
          abstractcontroller.h
          abstractscene.h
          commandcontroller.h
          editcontroller.h
          statemachinescene.h
          widgets/propertyeditor.h
          widgets/statemachinepalettewidget.h
          widgets/statemachineview.h
          widgets/statemachinetoolbar.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/view
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
