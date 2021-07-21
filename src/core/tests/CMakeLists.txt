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

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-test.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-test.h
)

# QtXmlPatterns fails badly on OSX -- see https://bugreports.qt-project.org/browse/QTBUG-40818
if(Qt${QT_MAJOR_VERSION}XmlPatterns_FOUND AND NOT APPLE)
  add_definitions(-DUSE_QT_XMLPATTERNS_LIB)
  list(APPEND KDSME_TESTHELPER_EXTRA_LIBS Qt${QT_MAJOR_VERSION}::XmlPatterns)
endif()

add_library(kdsme_testhelper STATIC
  parsehelper.cpp
  util.cpp
)
target_link_libraries(kdsme_testhelper
  Qt${QT_MAJOR_VERSION}::Test
  kdstatemachineeditor_core
)

if(GRAPHVIZ_FOUND)
  ecm_add_test(test_layouter.cpp
    LINK_LIBRARIES
      Qt${QT_MAJOR_VERSION}::Gui
      kdsme_testhelper
  )
endif()

ecm_add_test(test_layoutitem.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

ecm_add_test(test_models.cpp
  LINK_LIBRARIES
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Test
    kdstatemachineeditor_core
)

ecm_add_test(test_qmlexport.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

if(Qt${QT_MAJOR_VERSION}RemoteObjects_FOUND)
  set(SRCS test_qsmintegration.cpp)
  if (NOT BUILD_QT6)
    qt5_generate_repc(SRCS ../../debuginterface/debuginterface.rep REPLICA)
  endif()
  ecm_add_test(${SRCS} TEST_NAME test_qsmintegration
  LINK_LIBRARIES
    Qt${QT_MAJOR_VERSION}::RemoteObjects
    Qt${QT_MAJOR_VERSION}::Test
    kdstatemachineeditor_debuginterfaceclient
    kdstatemachineeditor_debuginterfacesource
    kdsme_testhelper
  )
  if (BUILD_QT6)
    target_link_libraries(test_qsmintegration Qt6::StateMachine)
    qt6_add_repc_replicas(test_qsmintegration ../../debuginterface/debuginterface.rep)
  endif()
endif()

ecm_add_test(test_scxmlimport.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

ecm_add_test(test_scxmlexport.cpp
  LINK_LIBRARIES
    Qt${QT_MAJOR_VERSION}::Test
    ${KDSME_TESTHELPER_EXTRA_LIBS}
    kdstatemachineeditor_core
)

ecm_add_test(test_statemachine.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

ecm_add_test(test_util.cpp
  LINK_LIBRARIES
    Qt${QT_MAJOR_VERSION}::Gui
    kdsme_testhelper
)

if (NOT BUILD_QT6 OR Qt6Core5Compat_FOUND)
  ecm_add_test(test_layoutinformation.cpp
    LINK_LIBRARIES
      Qt${QT_MAJOR_VERSION}::Gui
      Qt${QT_MAJOR_VERSION}::Test
      kdstatemachineeditor_core
  )
  if (BUILD_QT6)
    target_link_libraries(test_layoutinformation Qt6::Core5Compat)
  endif()
endif()
