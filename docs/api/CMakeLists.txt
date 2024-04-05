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

find_file(QDOC_QTCORE_TAG qtcore.tags HINTS ${QT_INSTALL_DOCS}/qtcore ${QT_INSTALL_DATA}/doc/qtcore)
set(QDOC_TAG_DIR "<QDOC_TAG_DIR_not_found>")
if(QDOC_QTCORE_TAG)
    get_filename_component(QDOC_TAG_DIR ${QDOC_QTCORE_TAG} DIRECTORY)
    get_filename_component(QDOC_TAG_DIR ${QDOC_TAG_DIR} DIRECTORY)
endif()

file(GLOB _dox_deps *.dox *.html)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

#apidox generation using doxygen
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

add_custom_command(
    OUTPUT ${DOXYGEN_OUTPUT_DIR}/qch/kdstatemachineeditor-api.qch
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    #copy some files by-hand that are referred to by the markdown README
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}/html/LICENSES
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/LICENSES/LGPL-2.1-only.txt
            ${DOXYGEN_OUTPUT_DIR}/html/LICENSES
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/LICENSES/LicenseRef-KDAB-KDStateMachineEditor.txt
            ${DOXYGEN_OUTPUT_DIR}/html/LICENSES
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}/html/docs
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/docs/KDStateMachineEditor-CopyrightAssignmentForm.pdf ${DOXYGEN_OUTPUT_DIR}/html/docs
    DEPENDS ${_dox_deps} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile ${CMAKE_SOURCE_DIR}/README.md
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generate the .qch file"
)
add_custom_target(
    kdstatemachineeditor-api.qch ALL
    DEPENDS ${DOXYGEN_OUTPUT_DIR}/qch/kdstatemachineeditor-api.qch
    COMMENT "Target generate the .qch file"
)
add_custom_target(
    docs
    DEPENDS kdstatemachineeditor-api.qch
    COMMENT "Target to generate the documentation"
)

set(QCH_INSTALL_DIR
    ${CMAKE_INSTALL_DOCDIR}${KDSME_LIBRARY_QTID}
    CACHE STRING "Install location of Qt Assistant help files."
)
install(FILES ${DOXYGEN_OUTPUT_DIR}/qch/kdstatemachineeditor-api.qch DESTINATION ${QCH_INSTALL_DIR})
install(FILES ${DOXYGEN_OUTPUT_DIR}/kdstatemachineeditor.tags DESTINATION ${CMAKE_INSTALL_DOCDIR})
