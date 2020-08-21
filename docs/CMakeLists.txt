#
# This file is part of the KDAB State Machine Editor Library.
#
# SPDX-FileCopyrightText: 2017-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

find_program(QHELPGEN_EXECUTABLE qhelpgenerator
  HINTS ${QT_INSTALL_BINS}
)

if(NOT QHELPGEN_EXECUTABLE)
  message(STATUS "Unable to build API documentation in qch format.")
  message(STATUS "qhelpgenerator exectuable: ${QHELPGEN_EXECUTABLE}")
endif()

# Doxygen
find_package(Doxygen)
set_package_properties(Doxygen PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "API Documentation system"
  URL "http://www.doxygen.org"
  PURPOSE "Needed to build the API documentation."
)
if(DOXYGEN_FOUND AND QHELPGEN_EXECUTABLE)
  add_subdirectory(api)
endif()
