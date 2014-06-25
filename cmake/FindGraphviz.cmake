# - Find Graphviz
# Find the Graphviz includes and libraries
# This module defines the IMPORTED targets if found:
#  Graphviz::dot - The dot executable
#  Graphviz::cdt - The cdt library
#  Graphviz::gvc - The gvc library
#  Graphviz::cgraph - The cgraph library
#  Graphviz::graph - The graph library
#  Graphviz::pathplan - The pathplan library
#
# All variables are internal. Consumers use the IMPORTED targets directly.

#=============================================================================
# Copyright 2014 Stephen Kelly <stephen.kelly@kdab.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

include(CheckIncludeFiles)

if (Graphviz_ROOT)
    set(_Graphviz_ROOT ${Graphviz_ROOT})
else()
    set(_Graphviz_ROOT $ENV{Graphviz_ROOT})
endif()

find_program(DOT_TOOL dot)

if (WIN32 AND NOT _Graphviz_ROOT AND NOT DOT_TOOL)
  message(STATUS "No way to find Graphviz. Set the path to the DOT_TOOL in the cache.")
  set(Graphviz_FOUND FALSE)
  return()
endif()

if(DOT_TOOL)
  add_executable(Graphviz::dot IMPORTED)
  set_property(TARGET Graphviz::dot PROPERTY IMPORTED_LOCATION ${DOT_TOOL})
  if(NOT _Graphviz_ROOT AND WIN32)
    get_filename_component(_Graphviz_ROOT ${DOT_TOOL} PATH)
  endif()
endif()

find_path(Graphviz_INCLUDE_DIR NAMES graphviz/graphviz_version.h
  HINTS ${_Graphviz_ROOT}
)

if(Graphviz_INCLUDE_DIR AND EXISTS "${Graphviz_INCLUDE_DIR}/graphviz/graphviz_version.h")
    file(STRINGS "${Graphviz_INCLUDE_DIR}/graphviz/graphviz_version.h" Graphviz_H REGEX "PACKAGE_VERSION")
    string(REGEX REPLACE "^#define[\t ]+PACKAGE_VERSION[\t ]+\"([^\"]*)\".*" "\\1" Graphviz_VERSION "${Graphviz_H}")

    string( REGEX REPLACE "([0-9]+).*" "\\1" Graphviz_VERSION_MAJOR "${Graphviz_VERSION}" )
    string( REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" Graphviz_VERSION_MINOR "${Graphviz_VERSION}" )
    string( REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" Graphviz_VERSION_PATCH "${Graphviz_VERSION}" )
elseif(DOT_TOOL)
    # Graphviz ships without graphviz_version.h on Windows.
    find_path(Graphviz_INCLUDE_DIR NAMES graphviz/graph.h graphviz/cgraph.h
      HINTS ${_Graphviz_ROOT}/include
    )
    execute_process(COMMAND ${DOT_TOOL} -V OUTPUT_VARIABLE DOT_VERSION_OUTPUT ERROR_VARIABLE DOT_VERSION_OUTPUT OUTPUT_QUIET)
    string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" Graphviz_VERSION "${DOT_VERSION_OUTPUT}")
    string(REPLACE "." ";" VL ${Graphviz_VERSION})
    list(GET VL 0 Graphviz_VERSION_MAJOR)
    list(GET VL 1 Graphviz_VERSION_MINOR)
    list(GET VL 2 Graphviz_VERSION_PATCH)
endif()

if(WIN32)
  set(Graphviz_LIB_PATH_SUFFIX_RELEASE "release/lib")
  set(Graphviz_LIB_PATH_SUFFIX_DEBUG "debug/lib")
endif()

set(gv_libs cdt gvc cgraph graph pathplan)
foreach(lib ${gv_libs})
  set(build_types RELEASE)
  if (WIN32)
    list(APPEND build_types DEBUG)
  endif()
  foreach(BUILD_TYPE ${build_types})
    string(TOUPPER ${lib} libupper)
    find_library(Graphviz_${libupper}_LIBRARY_${BUILD_TYPE} NAMES ${lib}
      HINTS ${_Graphviz_ROOT}/lib PATH_SUFFIXES ${Graphviz_LIB_PATH_SUFFIX_${BUILD_TYPE}})
  endforeach()
endforeach()

include("${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake")
find_package_handle_standard_args(Graphviz
                                  FOUND_VAR Graphviz_FOUND
                                  REQUIRED_VARS Graphviz_INCLUDE_DIR Graphviz_CDT_LIBRARY_RELEASE Graphviz_GVC_LIBRARY_RELEASE Graphviz_PATHPLAN_LIBRARY_RELEASE
                                  VERSION_VAR Graphviz_VERSION)

if(NOT Graphviz_CGRAPH_LIBRARY_RELEASE AND NOT Graphviz_GRAPH_LIBRARY_RELEASE)
  set(Graphviz_FOUND FALSE)
endif()

# work-around issue in Graphviz headers for >=2.26.0,  also see: https://github.com/KDAB/GammaRay/issues/79
set(_graphviz_extra_compile_definitions "")
if (NOT (Graphviz_VERSION VERSION_LESS 2.26.0))
    check_include_files(string.h have_string_h)
    if (have_string_h)
        list(APPEND _graphviz_extra_compile_definitions HAVE_STRING_H)
    endif()
endif()

foreach(lib ${gv_libs})
  string(TOUPPER ${lib} libupper)
  if (Graphviz_${libupper}_LIBRARY_RELEASE)
    add_library(Graphviz::${lib} UNKNOWN IMPORTED)
    set_property(TARGET Graphviz::${lib} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_property(TARGET Graphviz::${lib} PROPERTY INTERFACE_COMPILE_DEFINITIONS ${_graphviz_extra_compile_definitions})
    set_property(TARGET Graphviz::${lib} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Graphviz_INCLUDE_DIR})
    set_property(TARGET Graphviz::${lib} PROPERTY IMPORTED_LOCATION_RELEASE ${Graphviz_${libupper}_LIBRARY_RELEASE})
    if (Graphviz_${libupper}_LIBRARY_DEBUG)
      set_property(TARGET Graphviz::${lib} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
      set_property(TARGET Graphviz::${lib} PROPERTY IMPORTED_LOCATION_DEBUG ${Graphviz_${libupper}_LIBRARY_DEBUG})
    endif()
  endif()
endforeach()

unset(gv_libs)

if (TARGET Graphviz::cgraph)
  # you must add this define when using cgraph from graphviz
  # some headers check for this define (see for example graphviz/types.h header)
  set_property(TARGET Graphviz::cgraph APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS WITH_CGRAPH)
endif()
