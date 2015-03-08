#.rst:
# ECMAddTests
# -----------
#
# Add test executables.
#
# ::
#
#   ecm_add_test(<sources> LINK_LIBRARIES <library> [<library> [...]]
#                          [TEST_NAME <name>]
#                          [NAME_PREFIX <prefix>]
#                          [GUI])
#
# Add a new unit test using the passed source files. The parameter TEST_NAME is
# used to set the name of the resulting test, and the target built for and run
# by the test. It may be omitted if there is exactly one source file. In that
# case the name of the source file (without the file extension) will be used as
# the test name.
#
# If NAME_PREFIX is given, this prefix will be prepended to the test name, but
# not the target name. As a result, it will not prevent clashes between tests
# with the same name in different parts of the project, but it can be used to
# give an indication of where to look for a failing test.
#
# If the flag GUI is passed the test binary will be a GUI executable, otherwise
# the resulting binary will be a console application.  The test will be linked
# against the libraries and/or targets passed to LINK_LIBRARIES.
#
#
# ::
#
#   ecm_add_tests(<sources> LINK_LIBRARIES <library> [<library> [...]]
#                           [NAME_PREFIX <prefix>]
#                           [GUI])
#
# This is a convenient version of ecm_add_test() for when you have many tests
# that consist of a single source file each.  It behaves like calling
# ecm_add_test() once for each source file, with the same named arguments.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2013 Alexander Richardson <arichardson.kde@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file COPYING-CMAKE-SCRIPTS for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of extra-cmake-modules, substitute the full
#  License text for the above reference.)

include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)

function(ecm_add_test)
  set(options GUI)
  set(oneValueArgs TEST_NAME NAME_PREFIX)
  set(multiValueArgs LINK_LIBRARIES)
  cmake_parse_arguments(ECM_ADD_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  set(_sources ${ECM_ADD_TEST_UNPARSED_ARGUMENTS})
  list(LENGTH _sources _sourceCount)
  if(ECM_ADD_TEST_TEST_NAME)
    set(_targetname ${ECM_ADD_TEST_TEST_NAME})
  elseif(${_sourceCount} EQUAL "1")
    #use the source file name without extension as the testname
    get_filename_component(_targetname ${_sources} NAME_WE)
  else()
    #more than one source file passed, but no test name given -> error
    message(FATAL_ERROR "ecm_add_test() called with multiple source files but without setting \"TEST_NAME\"")
  endif()

  set(_testname "${ECM_ADD_TEST_NAME_PREFIX}${_targetname}")
  add_executable(${_targetname} ${_sources})
  if(NOT ECM_ADD_TEST_GUI)
    ecm_mark_nongui_executable(${_targetname})
  endif()
  add_test(NAME ${_testname} COMMAND ${_targetname})
  target_link_libraries(${_targetname} ${ECM_ADD_TEST_LINK_LIBRARIES})
  ecm_mark_as_test(${_targetname})
endfunction()

function(ecm_add_tests)
  set(options GUI)
  set(oneValueArgs NAME_PREFIX)
  set(multiValueArgs LINK_LIBRARIES)
  cmake_parse_arguments(ECM_ADD_TESTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(ECM_ADD_TESTS_GUI)
    set(_exe_type GUI)
  else()
    set(_exe_type "")
  endif()
  foreach(_test_source ${ECM_ADD_TESTS_UNPARSED_ARGUMENTS})
    ecm_add_test(${_test_source}
      NAME_PREFIX ${ECM_ADD_TESTS_NAME_PREFIX}
      LINK_LIBRARIES ${ECM_ADD_TESTS_LINK_LIBRARIES}
	  ${_exe_type}
    )
  endforeach()
endfunction()
