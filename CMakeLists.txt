# Top-level CMakeLists.txt file for the KDAB State Machine Editor Library.

cmake_minimum_required(VERSION 2.8.12) # because of ALIAS targets

project(kdstatemachineeditor)

# Version setup
set(KDSME_VERSION_MAJOR "0")
set(KDSME_VERSION_MINOR "0")
set(KDSME_VERSION_PATCH "95")
set(KDSME_VERSION "${KDSME_VERSION_MAJOR}.${KDSME_VERSION_MINOR}.${KDSME_VERSION_PATCH}")
set(KDSME_VERSION_STRING "${KDSME_VERSION}")
set(KDSME_SOVERSION "1.0.0")
set(PROJECT_VERSION_STRING "${KDSME_VERSION_STRING}")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})

include(CheckCXXCompilerFlag)
include(CMakePackageConfigHelpers)
include(CTest)
include(MacroLogFeature)
include(ECMAddTests)
include(ECMGeneratePriFile)

find_package(Qt5 5.3 REQUIRED Quick Test Widgets)
find_package(Qt5 5.3 REQUIRED QuickWidgets)
find_package(Qt5 5.3 OPTIONAL_COMPONENTS XmlPatterns)
set_package_properties(Qt5XmlPatterns PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Qt5 XmlPatterns library"
  PURPOSE "Required for unit tests dealing with XML input/output"
)

option(DEPLOY_DEPENDENCIES "Install Qt5 and graphviz" ON)
option(BUILD_EXAMPLES "Build examples directory" ON)

# Don't require lots of include_directories(${CMAKE_CURRENT_SOURCE_DIR}) etc.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Automatically include the current dirs in the INTERFACE_INCLUDE_DIRS of targets.
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(BIN_INSTALL_DIR bin)
set(LIB_INSTALL_DIR lib)
if(WIN32)
  set(LIB_INSTALL_DIR ${BIN_INSTALL_DIR})
endif()
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

set(GRAPHVIZ_MIN_VERSION "2.20")
find_package(Graphviz)
set_package_properties(Graphviz PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "Graph visualization software"
  PURPOSE "RECOMMENDED: Needed for automatic layout of state charts"
  URL "http://www.graphviz.org/"
)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-Wformat-security HAVE_GXX_FORMAT_SECURITY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wmissing-noreturn -Werror=return-type")
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

enable_testing()

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
add_custom_target(copy_data ALL)
add_custom_command(TARGET copy_data
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${PROJECT_BINARY_DIR}/data"
)

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
  COPY_ONLY
)

set(CMAKECONFIG_INSTALL_DIR lib/cmake/${CONFIG_DIR})
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
