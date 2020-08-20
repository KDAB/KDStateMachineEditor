set(QSMDEBUGINTERFACESOURCE_SRCS
  qsmdebuginterfacesource.cpp
  qsmwatcher.cpp

  ../../core/util/objecthelper.cpp
)

if(Qt5Scxml_FOUND)
  list(APPEND QSMDEBUGINTERFACESOURCE_SRCS qscxmldebuginterfacesource.cpp)
endif()

qt5_generate_repc(QSMDEBUGINTERFACESOURCE_SRCS ../debuginterface.rep SOURCE)

add_library(kdstatemachineeditor_debuginterfacesource STATIC ${QSMDEBUGINTERFACESOURCE_SRCS})
add_library(KDSME::DebugInterfaceSource ALIAS kdstatemachineeditor_debuginterfacesource)
target_link_libraries(kdstatemachineeditor_debuginterfacesource
  LINK_PRIVATE
    Qt5::RemoteObjects
    kdstatemachineeditor_core
  LINK_PUBLIC
    Qt5::Core
)
if(Qt5Scxml_FOUND)
  target_include_directories(kdstatemachineeditor_debuginterfacesource PRIVATE ${Qt5Scxml_PRIVATE_INCLUDE_DIRS})
  target_link_libraries(kdstatemachineeditor_debuginterfacesource
    LINK_PUBLIC
      Qt5::Scxml
  )
endif()
set_target_properties(kdstatemachineeditor_debuginterfacesource PROPERTIES
  SOVERSION ${KDSME_SOVERSION}
  VERSION ${KDSME_SOVERSION}
  EXPORT_NAME DebugInterfaceSource
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)
#version libraries on Windows
if(WIN32)
  set(postfix ${KDSME_SOVERSION})
  set(CMAKE_RELEASE_POSTFIX ${postfix})
  set_target_properties(kdstatemachineeditor_debuginterfacesource PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
  #append 'd' to debug libraries
  string(CONCAT postfix ${postfix} "d")
  set(CMAKE_DEBUG_POSTFIX ${postfix})
  set_target_properties(kdstatemachineeditor_debuginterfacesource PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
endif()

target_compile_definitions(kdstatemachineeditor_debuginterfacesource PRIVATE -Dkdstatemachineeditor_core_EXPORTS)
generate_export_header(kdstatemachineeditor_debuginterfacesource EXPORT_FILE_NAME kdsme_debuginterfacesource_export.h BASE_NAME KDSME_DEBUGINTERFACESOURCE)
target_include_directories(kdstatemachineeditor_debuginterfacesource
  PUBLIC
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>"
  PRIVATE
    ../
)
ecm_generate_pri_file(BASE_NAME KDSMEDebugInterfaceSource
                      LIB_NAME kdstatemachineeditor_debuginterfacesource
                      DEPS ""
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/debuginterfacesource
)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
install(TARGETS kdstatemachineeditor_debuginterfacesource EXPORT KDSME_TARGETS ${INSTALL_TARGETS_DEFAULT_ARGS})
# No need to install PDB, since there's no PDB generated, it's a static library.
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kdsme_debuginterfacesource_export.h
    qsmdebuginterfacesource.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/debuginterfacesource)
