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
# -DKDSME_INTERNAL_GRAPHVIZ=[true|false]
#  Allow to build with an external Graphviz install.
#  We build with our internal graphviz sub-module but someone might want to
#  build against distro package, which is not recommended and probably broken.
#  Default=true
#
# -KDSME_STATIC_GRAPHVIZ=[true|false]
#  Allow the internal Graphviz build to be statically.
#  Currently shared graphviz builds on Windows have link issues.
#  Default=true
#
# -DKDSME_DOCS=[true|false]
#  Build the documentation. Documentation is never built when cross-compiling.
#  Default=true
#
# -DKDSME_EXAMPLES=[true|false]
#  Build the examples. Examples are never built when cross-compiling.
#  Default=true
#
# -DBUILD_TESTING=[true|false]
#  Build the test harness. Tests are never built when cross-compiling.
#  Note: disabling tests also disables building the kdstatemachineeditor test application.
#  Default=True
#
# -DBUILD_SHARED_LIBS=[true|false]
#  Build shared libraries
#  Default=true

cmake_minimum_required(VERSION 3.16)

file(STRINGS version.txt KDSME_VERSION_FILE)
list(GET KDSME_VERSION_FILE 0 KDSME_VERSION)

project(
    kdstatemachineeditor
    DESCRIPTION "A framework for creating Qt State Machine metacode using a graphical user interface"
    HOMEPAGE_URL "https://github.com/KDAB/KDStateMachineEditor"
    LANGUAGES CXX C
    VERSION ${KDSME_VERSION}
)

set(KDSME_SOVERSION "2") #means the 2.x ABI is frozen. ABI changes will must go to version 3

if(POLICY CMP0177)
    # Silence cmake warning about normalizing paths in install()
    # Please add "make install" tests to CI before using the newer policy
    cmake_policy(SET CMP0177 OLD) # CMake normalizes install paths
endif()

include(FeatureSummary)

# Declare an option as renamed, and eventually update the old cache entry
function(renamed_option _old _new)
    get_property(
        _old_set
        CACHE ${_old}
        PROPERTY VALUE
        SET
    )
    if(_old_set)
        message(DEPRECATION "\"${_old}\" was renamed \"${_new}\". Cache entry will be updated.")
        set_property(CACHE ${_new} PROPERTY VALUE ${${_old}})
        unset(${_old} CACHE)
    endif()
endfunction()

if(CMAKE_CROSSCOMPILING)
    set(KDSME_DOCS OFF)
    set(KDSME_EXAMPLES OFF)
    set(BUILD_TESTING OFF)
else()
    option(KDSME_DOCS "Build KDStateMachineEditor documentation" ON)
    option(KDSME_EXAMPLES "Build examples directory" ON)
    option(BUILD_TESTING "Build the test harness" ON)
endif()
renamed_option(BUILD_DOCS KDSME_DOCS)
renamed_option(BUILD_EXAMPLES KDSME_EXAMPLES)
renamed_option(BUILD_TESTS BUILD_TESTING)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

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

set(QT_VERSION_MAJOR 6)
set(QT_MIN_VERSION "6.1.0")
find_package(
    Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Quick QuickControls2 QuickWidgets Test Widgets StateMachine
)
set(KDSME_LIBRARY_QTID "-qt6")

include(KDQtInstallPaths) #to set QT_INSTALL_FOO variables

find_package(Qt${QT_VERSION_MAJOR}RemoteObjects ${QT_MIN_VERSION} CONFIG QUIET)
set_package_properties(
    Qt${QT_VERSION_MAJOR}RemoteObjects PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "Qt Remote Objects module"
    PURPOSE "Needed for the QStateMachine/QtSCXML adapter and remote debugging capabilities"
)

find_package(Qt${QT_VERSION_MAJOR}Scxml ${QT_MIN_VERSION} CONFIG QUIET)
set_package_properties(
    Qt${QT_VERSION_MAJOR}Scxml PROPERTIES
    TYPE OPTIONAL
    DESCRIPTION "Qt SCXML module"
    PURPOSE "Needed for the Qt SCXML adapter (adapter itself depends on Qt RemoteObjects)"
)

if(Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
    set(QT_NO_PRIVATE_MODULE_WARNING ON)
    find_package(Qt6GuiPrivate REQUIRED NO_MODULE)
    find_package(Qt6ScxmlPrivate REQUIRED NO_MODULE)
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

set(INCLUDE_INSTALL_ROOT ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}${KDSME_LIBRARY_QTID})

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
    "/Applications/Qt${QT_VERSION_MAJOR}"
)

option(KDSME_INTERNAL_GRAPHVIZ "Enable internal build of external project Graphviz" ON)
renamed_option(WITH_INTERNAL_GRAPHVIZ KDSME_INTERNAL_GRAPHVIZ)
add_feature_info("Internal build of Graphviz" KDSME_INTERNAL_GRAPHVIZ "disable with KDSME_INTERNAL_GRAPHVIZ=OFF")

if(KDSME_INTERNAL_GRAPHVIZ)
    option(KDSME_STATIC_GRAPHVIZ "Enable static build of Graphviz when internally building" ON)
    renamed_option(WITH_STATIC_GRAPHVIZ KDSME_STATIC_GRAPHVIZ)
    add_feature_info(
        "Statically build Graphviz for internal builds" KDSME_INTERNAL_GRAPHVIZ
        "disable with KDSME_STATIC_GRAPHVIZ=OFF"
    )
    set(GRAPHVIZ_FOUND ON)

    include_directories(${PROJECT_SOURCE_DIR}/src/fwd_headers)
    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/graphviz/lib/common)
    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/graphviz/lib/gvc)
    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/graphviz/lib/pathplan)
    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/graphviz/lib/cgraph)
    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/graphviz/lib/cdt)
else()
    # search for External Graphviz installation, NOT SUPPORTED anymore
    set(GRAPHVIZ_MIN_VERSION "2.30.1")
    find_package(Graphviz)
    set_package_properties(
        Graphviz PROPERTIES
        TYPE RECOMMENDED
        DESCRIPTION "Graph visualization software"
        PURPOSE "Needed for automatic layout of state charts"
        URL "https://www.graphviz.org/"
    )
endif()

add_subdirectory(3rdparty EXCLUDE_FROM_ALL)

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
if((CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND NOT DEFINED CMAKE_CXX_CLANG_TIDY)
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
    if(MINGW)
        # mingw will error out on the crazy casts in probe.cpp without this
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
    endif()
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER MATCHES "clazy")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -Wdocumentation")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qunused-arguments")
endif()

add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_USE_FAST_OPERATOR_PLUS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x060900
)

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
    set(BUILD_TESTING FALSE)
    set(KDSME_EXAMPLES FALSE)
    set(KDSME_DOCS FALSE)
endif()

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
endif()

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

add_subdirectory(src)

if(KDSME_EXAMPLES)
    add_subdirectory(examples)
endif()

if(KDSME_DOCS)
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

add_custom_target(
    kdsme_cppcheck
    USES_TERMINAL
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "run cppcheck on source files"
    COMMAND
        ${CMAKE_COMMAND} -E env cppcheck "--project=${PROJECT_BINARY_DIR}/compile_commands.json" --enable=all
        --error-exitcode=1 --language=c++ --inline-suppr --quiet --disable=missingInclude,unusedFunction
        --check-level=exhaustive --library=qt.cfg --suppress=*:*.moc --suppress=*:*moc_*.cpp
        "-i${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/" "-i${CMAKE_CURRENT_BINARY_DIR}/3rdparty/"
        "--suppress=*:${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/" "--suppress=*:${CMAKE_CURRENT_BINARY_DIR}/3rdparty/"
)
