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

# This is the top-level CMakeLists.txt file for the KDStateMachineEditor project.
#
# Pass the following variables to cmake to control the build:
# (See INSTALL.txt for more information)
#
# -DWITH_INTERNAL_GRAPHVIZ=[true|false]
#  Allow to build with an internal Graphviz install.
#  In the case where a separate Graphviz installation is not practical or desired.
#  Default=true if Graphviz is not located; false otherwise
#
# -DBUILD_QT6=[true|false]
#  Build against Qt6 rather than Qt5
#  Default=false (Qt5 will be used even if Qt6 is available)
#
# -DBUILD_DOCS=[true|false]
#  Build the documentation. Documentation is never built when cross-compiling.
#  Default=true
#
# -DBUILD_EXAMPLES=[true|false]
#  Build the examples. Examples are never built when cross-compiling.
#  Default=true
#
# -DBUILD_TESTS=[true|false]
#  Build the test harness. Tests are never built when cross-compiling.
#  Note: disabling tests also disables building the kdstatemachineeditor test application.
#  Default=True
#

cmake_minimum_required(VERSION 3.16)

project(
    kdstatemachineeditor
    DESCRIPTION "A framework for creating Qt State Machine metacode using a graphical user interface"
    HOMEPAGE_URL "https://github.com/KDAB/KDStateMachineEditor"
    LANGUAGES CXX C
    VERSION 1.99.50
)

add_definitions(
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
    -DQT_USE_FAST_OPERATOR_PLUS
)

set(KDSME_SOVERSION "2") #means the 2.x ABI is frozen. ABI changes will must go to version 3

include(FeatureSummary)

if(CMAKE_CROSSCOMPILING)
    set(BUILD_DOCS OFF)
    set(BUILD_EXAMPLES OFF)
    set(BUILD_TESTS OFF)
else()
    option(BUILD_DOCS "Build KDStateMachineEditor documentation" ON)
    option(BUILD_EXAMPLES "Build examples directory" ON)
    option(BUILD_TESTS "Build the test harness" ON)
endif()
option(BUILD_QT6 "Build against Qt 6" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/KDAB/modules")

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to ${default_build_type} as none was specified.")
    set(CMAKE_BUILD_TYPE
        "${default_build_type}"
        CACHE STRING "Choose the type of build." FORCE
    )
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(BUILD_QT6)
    set(Qt_VERSION_MAJOR 6)
    set(QT_MIN_VERSION "6.1.0")
    find_package(
        Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Quick QuickControls2 QuickWidgets Test Widgets StateMachine
    )
    find_package(Qt6Core5Compat ${QT_MIN_VERSION} CONFIG QUIET)
    set_package_properties(
        Qt6Core5Compat PROPERTIES
        TYPE OPTIONAL
        DESCRIPTION "Qt6-Qt5 compatibility module"
        PURPOSE "Needed for the layoutinformation unit test"
    )
    set(KDSME_LIBRARY_QTID "-qt6")
else()
    set(Qt_VERSION_MAJOR 5)
    set(QT_MIN_VERSION "5.15")
    find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Quick QuickWidgets Test Widgets)
    set(KDSME_LIBRARY_QTID "")
    include(ECMGeneratePriFile)
    include(KDQtInstallPaths) #to set QT_INSTALL_FOO variables
endif()

find_package(Qt${Qt_VERSION_MAJOR}RemoteObjects ${QT_MIN_VERSION} CONFIG QUIET)
set_package_properties(
    Qt${Qt_VERSION_MAJOR}RemoteObjects PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "Qt Remote Objects module"
    PURPOSE "Needed for the QStateMachine/QtSCXML adapter and remote debugging capabilities"
)

find_package(Qt${Qt_VERSION_MAJOR}Scxml ${QT_MIN_VERSION} CONFIG QUIET)
set_package_properties(
    Qt${Qt_VERSION_MAJOR}Scxml PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "Qt SCXML module"
    PURPOSE "Needed for the Qt SCXML adapter (adapter itself depends on Qt RemoteObjects)"
)

# QtXmlPatterns is removed since Qt6
if(NOT BUILD_QT6)
    find_package(Qt5XmlPatterns ${QT_MIN_VERSION} CONFIG QUIET)
    set_package_properties(
        Qt5XmlPatterns PROPERTIES
        TYPE OPTIONAL
        DESCRIPTION "Qt5 XmlPatterns library"
        PURPOSE "Required with Qt5 for unit tests dealing with XML input/output"
    )
endif()

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckIncludeFiles)
include(CMakePackageConfigHelpers)
include(CTest)
include(GenerateExportHeader)
include(GNUInstallDirs)
include(ExternalProject)
include(ECMAddTests)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(TARGET Qt6::Core)
    set(CMAKE_CXX_STANDARD 17)
endif()

if(NOT DEFINED CMAKE_MACOSX_RPATH)
    set(CMAKE_MACOSX_RPATH TRUE)
endif()
if(NOT DEFINED CMAKE_INSTALL_RPATH_USE_LINK_PATH)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

if(WIN32)
    # Needed for qmake-integration because qmake's QT modules import mechanism does assume
    # debug libraries at windows always have a d suffix.
    set(CMAKE_DEBUG_POSTFIX
        "d"
        CACHE STRING "debug library postfix, usually d on windows"
    )
endif()

set(BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
set(LIB_INSTALL_DIR
    ${CMAKE_INSTALL_LIBDIR}
    CACHE STRING "Library install destination."
)
if(BUILD_QT6)
    set(INCLUDE_INSTALL_ROOT ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}${KDSME_LIBRARY_QTID})
else()
    set(INCLUDE_INSTALL_ROOT ${CMAKE_INSTALL_INCLUDEDIR}/)
endif()
set(INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_ROOT}/${CMAKE_PROJECT_NAME})
set(XDG_DATA_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/${CMAKE_PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

set(INSTALL_TARGETS_DEFAULT_ARGS
    RUNTIME
    DESTINATION
    ${BIN_INSTALL_DIR}
    LIBRARY
    DESTINATION
    ${LIB_INSTALL_DIR}
    ARCHIVE
    DESTINATION
    ${LIB_INSTALL_DIR}
    COMPONENT
    Devel
    BUNDLE
    DESTINATION
    "/Applications/Qt${Qt_VERSION_MAJOR}"
)

# search for Graphviz
set(GRAPHVIZ_MIN_VERSION "2.30.1")
find_package(Graphviz)
set_package_properties(
    Graphviz PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Graph visualization software"
    PURPOSE "Needed for automatic layout of state charts"
    URL "https://www.graphviz.org/"
)

if(GRAPHVIZ_FOUND)
    set(WITH_INTERNAL_GRAPHVIZ_DEFAULT OFF)
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL QNX)
    message(STATUS "Auto-enabling internal Graphviz build, since no installed version is available")
    set(WITH_INTERNAL_GRAPHVIZ_DEFAULT ON)
endif()

option(WITH_INTERNAL_GRAPHVIZ "Enable build of external project Graphviz" ${WITH_INTERNAL_GRAPHVIZ_DEFAULT})
add_feature_info("Internal build of Graphviz" WITH_INTERNAL_GRAPHVIZ "enable with WITH_INTERNAL_GRAPHVIZ=ON")

set(WITH_STATIC_GRAPHVIZ OFF) # Whether the Graphviz build we're using is static or not -- WIP

if(WITH_INTERNAL_GRAPHVIZ)
    # CMake is really awful here...
    set(source_dir ${CMAKE_CURRENT_BINARY_DIR}/graphviz-src)
    set(install_dir ${CMAKE_CURRENT_BINARY_DIR}/graphviz-install)

    # work-around: https://cmake.org/Bug/view.php?id=15052
    set(include_dir "${install_dir}/include")
    file(MAKE_DIRECTORY ${include_dir})
    set(GRAPHVIZ_INCLUDE_DIR ${include_dir})

    if(UNIX OR WITH_STATIC_GRAPHVIZ)
        set(libfolder "lib")
    else()
        set(libfolder "bin")
    endif()

    if(WITH_STATIC_GRAPHVIZ)
        set(staticdotlayoutplugin_library
            "${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gvplugin_dot_layout${CMAKE_STATIC_LIBRARY_SUFFIX}"
        )
        set(link_libraries_absolute)
        foreach(link_library "xdot;gvc;cgraph;pathplan")
            list(APPEND link_libraries_absolute
                 ${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${link_library}${CMAKE_STATIC_LIBRARY_SUFFIX}
            )
        endforeach()
    else()
        set(GRAPHVIZ_CGRAPH_LIBRARY
            "${install_dir}/${libfolder}/${CMAKE_SHARED_LIBRARY_PREFIX}cgraph${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
        set(GRAPHVIZ_GVC_LIBRARY
            "${install_dir}/${libfolder}/${CMAKE_SHARED_LIBRARY_PREFIX}gvc${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
        set(link_libraries_absolute "${GRAPHVIZ_CGRAPH_LIBRARY};${GRAPHVIZ_GVC_LIBRARY}")
    endif()

    set(win_build_utilities ${source_dir}/windows/dependencies/graphviz-build-utilities)
    string(TIMESTAMP graphvizDate)
    set(GRAPHVIZ_MAJOR_VERSION 2)
    set(GRAPHVIZ_MINOR_VERSION 48)
    set(GRAPHVIZ_PATCH_VERSION 0)
    set(graphvizVersion "${GRAPHVIZ_MAJOR_VERSION}.${GRAPHVIZ_MINOR_VERSION}.${GRAPHVIZ_PATCH_VERSION}")
    # cmake-lint: disable=C0301
    externalproject_add(
        GraphvizExternalProject
        GIT_REPOSITORY "https://gitlab.com/graphviz/graphviz.git"
        GIT_TAG ${graphvizVersion}
        GIT_PROGRESS 1
        USES_TERMINAL_DOWNLOAD
            TRUE # needed for Ninja so we can see the Git progress (cf. https://gitlab.kitware.com/cmake/cmake/-/issues/18238)
        CMAKE_ARGS
            -DVERSION=${graphvizVersion}
            -DDATE=${graphvizDate}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_C_COMPILER_AR=${CMAKE_C_COMPILER_AR}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_COMPILER_AR=${CMAKE_CXX_COMPILER_AR}
            -DCMAKE_INSTALL_PREFIX=${install_dir}
            $<$<BOOL:${WIN32}>:-DCMAKE_IGNORE_PATH="C:/Strawberry/c/bin">
            $<$<BOOL:${WIN32}>:-DCMAKE_SYSTEM_PREFIX_PATH=${win_build_utilities}$<SEMICOLON>${win_build_utilities}/winflexbison$<SEMICOLON>${win_build_utilities}/Perl64$<SEMICOLON>${win_build_utilities}/GnuWin/bin>
        BUILD_COMMAND "" # skip, make install is enough
        SOURCE_DIR ${source_dir}
        INSTALL_DIR ${install_dir}
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
        # Note: BUILD_BYPRODUCTS only available since CMake 3.2
        # Needed to make ExternalProject_Add in combination with the Ninja generator work
        # Also see: https://cmake.org/pipermail/cmake/2015-April/060234.html
        BUILD_BYPRODUCTS ${staticdotlayoutplugin_library} ${link_libraries_absolute}
    )

    if(WITH_STATIC_GRAPHVIZ)
        add_library(GraphvizStaticDotLayoutPlugin IMPORTED STATIC)
        set_target_properties(
            GraphvizStaticDotLayoutPlugin
            PROPERTIES IMPORTED_LOCATION "${staticdotlayoutplugin_library}"
                       INTERFACE_COMPILE_DEFINITIONS
                       "CGRAPH_EXPORTS;GVC_EXPORTS;WITH_CGRAPH" # Fix export macro madness in Graphviz
                       INTERFACE_INCLUDE_DIRECTORIES ${include_dir}
                       INTERFACE_LINK_LIBRARIES "${link_libraries_absolute}"
        )
    else()
        # in case of a Graphviz dynamic library build, we need to install
        install(DIRECTORY ${install_dir}/ DESTINATION ".")
    endif()

    set(GRAPHVIZ_FOUND TRUE)
endif()

#
# Compiler & linker settings
#

# If the condition is true, add the specified value to the arguments at the parent scope
function(append_if condition value)
    if(${condition})
        foreach(variable ${ARGN})
            set(${variable}
                "${${variable}} ${value}"
                PARENT_SCOPE
            )
        endforeach()
    endif()
endfunction()

# Add C and C++ compiler command line option
macro(add_flag_if_supported flag name)
    check_c_compiler_flag("-Werror ${flag}" "C_SUPPORTS_${name}")
    append_if("C_SUPPORTS_${name}" "${flag}" CMAKE_C_FLAGS)
    check_cxx_compiler_flag("-Werror ${flag}" "CXX_SUPPORTS_${name}")
    append_if("CXX_SUPPORTS_${name}" "${flag}" CMAKE_CXX_FLAGS)
endmacro()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
if(CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_flag_if_supported(-Wunused-but-set-variable UNUSED_BUT_SET)
    add_flag_if_supported(-Wlogical-op LOGICAL_OP)
    add_flag_if_supported(-Wsizeof-pointer-memaccess POINTER_MEMACCESS)
    add_flag_if_supported(-Wreorder REORDER)
    add_flag_if_supported(-Wformat-security FORMAT_SECURITY)

    check_cxx_compiler_flag(-std=gnu++0x HAVE_GXX_GNUXX11)
    check_cxx_compiler_flag(-std=c++0x HAVE_GXX_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith -Wmissing-noreturn -Werror=return-type -Wswitch")
    if(NOT BUILD_QT6)
        if(HAVE_GXX_GNUXX11) # QNX needs gnu++0x rather than c++0x for compiling QML V4 private headers
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
        elseif(HAVE_GXX_CXX11)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        endif()
    endif()
    if(MINGW)
        # mingw will error out on the crazy casts in probe.cpp without this
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
    endif()
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -Wdocumentation")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qunused-arguments")
endif()
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050400)
if(MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

# linker flags
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
        set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
    endif()
endif()

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(${PROJECT_NAME}_IS_ROOT_PROJECT TRUE)

    message(STATUS "Building ${PROJECT_NAME} ${PROJECT_VERSION} in ${CMAKE_BUILD_TYPE} mode. "
                   "Installing to ${CMAKE_INSTALL_PREFIX}"
    )
else()
    #Always disable tests, examples, docs when used as a submodule
    set(${PROJECT_NAME}_IS_ROOT_PROJECT FALSE)
    set(BUILD_TESTS FALSE)
    set(BUILD_EXAMPLES FALSE)
    set(BUILD_DOCS FALSE)
endif()

if(BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
endif()

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

set(CONFIG_DIR "KDSME${KDSME_LIBRARY_QTID}")
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/${CONFIG_DIR})

# Generate library version files
include(ECMSetupVersion)
ecm_setup_version(
    ${PROJECT_VERSION}
    VARIABLE_PREFIX
    KDSME
    VERSION_HEADER
    "${CMAKE_CURRENT_BINARY_DIR}/kdsme-version.h"
    PACKAGE_VERSION_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DIR}/KDSME${KDSME_LIBRARY_QTID}ConfigVersion.cmake"
    SOVERSION
    ${KDSME_SOVERSION}
    COMPATIBILITY
    AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kdsme-version.h" DESTINATION ${INCLUDE_INSTALL_DIR})

configure_file(
    cmake/KDSMEConfig.cmake "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DIR}/KDSME${KDSME_LIBRARY_QTID}Config.cmake" @ONLY
)

install(
    EXPORT KDSME_TARGETS
    FILE KDSME${KDSME_LIBRARY_QTID}Targets.cmake
    NAMESPACE KDSME::
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DIR}/KDSME${KDSME_LIBRARY_QTID}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DIR}/KDSME${KDSME_LIBRARY_QTID}ConfigVersion.cmake"
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    COMPONENT Devel
)

if(${PROJECT_NAME}_IS_ROOT_PROJECT)
    set(LICENSE_FILE "LGPL-2.1-only.txt")
    set(README_FILE "README.md")

    #CPACK
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_VENDOR "KDAB")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${README_FILE}")
    if(WIN32)
        set(CPACK_GENERATOR "NSIS")
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "KDStateMachineEditor")
        set(CPACK_PACKAGE_FILE_NAME "KDStateMachineEditor-${PROJECT_VERSION}")
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSES/${LICENSE_FILE}")
        set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}")
        set(CPACK_NSIS_URL_INFO_ABOUT "https://www.kdab.com/")
        set(CPACK_NSIS_MENU_LINKS "${LICENSE_FILE}" "License" "${README_FILE}" "Readme")
    endif()
    include(CPack)

    # Add uninstall target
    include(ECMUninstallTarget)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
