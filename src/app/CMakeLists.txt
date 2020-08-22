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

set(KDSME_APPBIN ${CMAKE_PROJECT_NAME})

set(BIN_SRCS
  mainwindow.cpp
  main.cpp
)
set(BIN_UIS
  mainwindow.ui
)

qt5_wrap_ui(BIN_SRCS ${BIN_UIS})

add_executable(${KDSME_APPBIN} WIN32 ${BIN_SRCS})
target_link_libraries(${KDSME_APPBIN}
  Qt5::QuickWidgets
  kdstatemachineeditor_core
  kdstatemachineeditor_view
  kdsme_testhelper
)
