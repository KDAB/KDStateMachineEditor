# Top-level CMakeLists.txt file for the KDAB State Machine Editor Library.

cmake_minimum_required(VERSION 2.8.12) # because of ALIAS targets

project(kdstatemachineeditor)

# Version setup
set(KDSME_VERSION_MAJOR "1")
set(KDSME_VERSION_MINOR "0")
set(KDSME_VERSION_PATCH "0")
set(KDSME_VERSION "${KDSME_VERSION_MAJOR}.${KDSME_VERSION_MINOR}.${KDSME_VERSION_PATCH}")
set(KDSME_VERSION_STRING "${KDSME_VERSION}")
set(KDSME_SOVERSION "1.0.0")
set(PROJECT_VERSION_STRING "${KDSME_VERSION_STRING}")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

if(WIN32)
  # Needed for qmake-integration because qmake's QT modules import mechanism does assume
  # debug libraries at windows always have a d suffix.
  set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "debug library postfix, usually d on windows")
endif()

include(CheckCXXCompilerFlag)
include(CheckIncludeFiles)
include(CMakePackageConfigHelpers)
include(CTest)
include(GenerateExportHeader)
include(ExternalProject)
include(MacroLogFeature)
include(ECMAddTests)
include(ECMGeneratePriFile)

find_package(Qt5 5.3 REQUIRED Quick Test Widgets)
find_package(Qt5 5.3 REQUIRED QuickWidgets)

find_package(Qt5RemoteObjects 5.4 QUIET)
set_package_properties(Qt5RemoteObjects PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Qt Remote Objects module"
  PURPOSE "Needed for the QStateMachine adapter and remote debugging capabilities"
)

find_package(Qt5 5.3 OPTIONAL_COMPONENTS XmlPatterns)
set_package_properties(Qt5XmlPatterns PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Qt5 XmlPatterns library"
  PURPOSE "Required for unit tests dealing with XML input/output"
)

option(DEPLOY_DEPENDENCIES "Install Qt5 and graphviz" ON)

if(NOT CMAKE_CROSSCOMPILING)
  option(BUILD_EXAMPLES "Build examples directory" ON)
endif()

# Don't require lots of include_directories(${CMAKE_CURRENT_SOURCE_DIR}) etc.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Automatically include the current dirs in the INTERFACE_INCLUDE_DIRS of targets.
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_AUTOMOC TRUE)

set(BIN_INSTALL_DIR bin)
set(LIB_INSTALL_DIR lib CACHE STRING "Library install destination.")
set(INCLUDE_INSTALL_DIR include/${CMAKE_PROJECT_NAME})
set(XDG_DATA_INSTALL_DIR share/${CMAKE_PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

set(INSTALL_TARGETS_DEFAULT_ARGS
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
  BUNDLE DESTINATION "/Applications/Qt5"
)

# There is no 64-bit Graphviz available for Windows.
include(CheckTypeSize)
check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)

option(WITH_INTERNAL_GRAPHVIZ "Enable build of external project Graphviz" OFF)
add_feature_info("Internal build of Graphviz" WITH_INTERNAL_GRAPHVIZ "enable with WITH_INTERNAL_GRAPHVIZ=ON")

if(WITH_INTERNAL_GRAPHVIZ)
    ExternalProject_Add(GraphvizExternalProject
        GIT_REPOSITORY "https://github.com/krf/graphviz.git"
        GIT_TAG "ed87878d212ec879e77f632726392a14b0844b4d"
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/graphviz-install
        INSTALL_COMMAND
            ${CMAKE_COMMAND} --build . --target install
    )
    ExternalProject_Get_Property(GraphvizExternalProject source_dir)
    ExternalProject_Get_Property(GraphvizExternalProject binary_dir)
    ExternalProject_Get_Property(GraphvizExternalProject install_dir)

    # work-around: https://cmake.org/Bug/view.php?id=15052
    set(include_dir "${install_dir}/include")
    file(MAKE_DIRECTORY ${include_dir})

    # CMake is really awful here...
    set(link_libraries "dotgen;common;xdot;gvc;cgraph;cdt;pathplan;ortho;pack;label;ingraphs")
    set(link_libraries_absolute)
    foreach(link_library ${link_libraries})
        list(APPEND link_libraries_absolute ${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${link_library}${CMAKE_STATIC_LIBRARY_SUFFIX})
    endforeach()

    add_library(GraphvizStaticDotLayoutPlugin IMPORTED STATIC)
    set_target_properties(GraphvizStaticDotLayoutPlugin PROPERTIES
        IMPORTED_LOCATION "${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gvplugin_dot_layout${CMAKE_STATIC_LIBRARY_SUFFIX}"
        INTERFACE_COMPILE_DEFINITIONS "CGRAPH_EXPORTS;GVC_EXPORTS;WITH_CGRAPH" # Fix export macro madness in Graphviz
        INTERFACE_INCLUDE_DIRECTORIES ${include_dir}
        INTERFACE_LINK_LIBRARIES "${link_libraries_absolute}"
    )
    add_dependencies(GraphvizStaticDotLayoutPlugin GraphvizExternalProject)
    set(GRAPHVIZ_FOUND TRUE)
    set(GRAPHVIZ_MAJOR_VERSION 2)
    set(GRAPHVIZ_MINOR_VERSION 38)
    set(GRAPHVIZ_PATCH_VERSION 0)
else()
  # search for Graphviz
  if(WIN32 AND SIZEOF_VOID_P EQUAL 8)
    set(GRAPHVIZ_FOUND FALSE)
    add_feature_info("Graph visualization software" FALSE "A 64-bit Graphviz library does not exist for Windows at this time.")
  else()
    set(GRAPHVIZ_MIN_VERSION "2.20")
    find_package(Graphviz)
    set_package_properties(Graphviz PROPERTIES
      TYPE RECOMMENDED
      DESCRIPTION "Graph visualization software"
      PURPOSE "Needed for automatic layout of state charts"
      URL "http://www.graphviz.org/"
    )
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-Wformat-security HAVE_GXX_FORMAT_SECURITY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wswitch -Wpointer-arith -Wmissing-noreturn -Werror=return-type")
  if(HAVE_GXX_UNUSED_BUT_SET)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GXX_LOGICAL_OP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
  endif()
  if(HAVE_GXX_POINTER_MEMACCESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsizeof-pointer-memaccess")
  endif()
  if(HAVE_GXX_REORDER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreorder")
  endif()
  if(HAVE_GXX_FORMAT_SECURITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wformat-security")
  endif()
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") # required in C++11 mode
  endif()
endif()

# linker flags
if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
  if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
  endif()
endif()

if(NOT CMAKE_CROSSCOMPILING)
  enable_testing()
endif()

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
set_package_properties(Doxygen PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Doxygen documentation generator"
  PURPOSE "Needed for generating API documentation (make docs)"
)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(docs
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
add_subdirectory(src)

set(CONFIG_DIR "KDSME")
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DIR}/KDSMEConfigVersion.cmake"
  VERSION ${KDSME_VERSION}
  COMPATIBILITY AnyNewerVersion
)
configure_file(cmake/KDSMEConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DIR}/KDSMEConfig.cmake"
  COPYONLY
)

set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/${CONFIG_DIR})
install(EXPORT KDSME_TARGETS
  NAMESPACE
    KDSME::
  FILE
    KDSMETargets.cmake
  DESTINATION
    ${CMAKECONFIG_INSTALL_DIR}
)
install(
  FILES
    cmake/KDSMEConfig.cmake
    cmake/KDSMEFindDependencyMacro.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DIR}/KDSMEConfigVersion.cmake"
  DESTINATION
    ${CMAKECONFIG_INSTALL_DIR}
  COMPONENT
    Devel
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
