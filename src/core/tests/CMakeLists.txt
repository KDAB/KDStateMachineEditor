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
if(Qt5XmlPatterns_FOUND AND NOT APPLE)
  add_definitions(-DUSE_QT_XMLPATTERNS_LIB)
  list(APPEND KDSME_TESTHELPER_EXTRA_LIBS ${Qt5XmlPatterns_LIBRARIES})
endif()

add_library(kdsme_testhelper STATIC
  parsehelper.cpp
  util.cpp
)
target_link_libraries(kdsme_testhelper
  Qt5::Test
  kdstatemachineeditor_core
)

if(GRAPHVIZ_FOUND)
  ecm_add_test(test_layouter.cpp
    LINK_LIBRARIES
      Qt5::Gui
      kdsme_testhelper
  )
endif()

ecm_add_test(test_layoutitem.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

ecm_add_test(test_models.cpp
  LINK_LIBRARIES
    Qt5::Gui
    Qt5::Test
    kdstatemachineeditor_core
)

ecm_add_test(test_qmlexport.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

if(Qt5RemoteObjects_FOUND)
  set(SRCS test_qsmintegration.cpp)
  qt5_generate_repc(SRCS ../../debuginterface/debuginterface.rep REPLICA)
  ecm_add_test(${SRCS} TEST_NAME test_qsmintegration
  LINK_LIBRARIES
    Qt5::RemoteObjects
    Qt5::Test
    kdstatemachineeditor_debuginterfaceclient
    kdstatemachineeditor_debuginterfacesource
    kdsme_testhelper
  )
endif()

ecm_add_test(test_scxmlimport.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

ecm_add_test(test_scxmlexport.cpp
  LINK_LIBRARIES
    Qt5::Test
    ${KDSME_TESTHELPER_EXTRA_LIBS}
    kdstatemachineeditor_core
)

ecm_add_test(test_statemachine.cpp
  LINK_LIBRARIES
    kdsme_testhelper
)

ecm_add_test(test_util.cpp
  LINK_LIBRARIES
    Qt5::Gui
    kdsme_testhelper
)

ecm_add_test(test_layoutinformation.cpp
  LINK_LIBRARIES
    Qt5::Gui
    Qt5::Test
    kdstatemachineeditor_core
)
