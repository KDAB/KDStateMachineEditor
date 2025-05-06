# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Allen Winter <allen.winter@kdab.com>
#
# SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor
#
# Licensees holding valid commercial KDAB State Machine Editor Library
# licenses may use this file in accordance with the KDAB State Machine Editor
# Library License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.
#

# qhelpgenerator
if(NOT DEFINED QT_INSTALL_BINS)
    # silent --warn-uninitialized
    set(QT_INSTALL_BINS "")
endif()

find_program(QHELPGEN_EXECUTABLE qhelpgenerator HINTS ${QT_INSTALL_BINS} "${QT_INSTALL_BINS}/../libexec")

if(QHELPGEN_EXECUTABLE)
    set(HAVE_QHELPGEN "YES")
else()
    set(HAVE_QHELPGEN "NO")
    message(STATUS "Unable to generate the API documentation in qch format. "
                   "To fix, install the qhelpgenerator program which comes with Qt."
    )
endif()

# Doxygen
find_package(Doxygen)
set_package_properties(
    Doxygen PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "API Documentation system"
    URL "https://www.doxygen.org"
    PURPOSE "Needed to build the API documentation."
)
if(DOXYGEN_DOT_EXECUTABLE)
    set(HAVE_DOT "YES")
else()
    set(HAVE_DOT "NO")
    message(STATUS "Unable to provide inheritance diagrams for the API documentation. "
                   "To fix, install the graphviz project from https://www.graphviz.org"
    )
endif()

if(DOXYGEN_FOUND AND QHELPGEN_EXECUTABLE)
    add_subdirectory(api)
else()
    message(STATUS "Not adding api directory!")
endif()
