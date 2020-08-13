set(DEBUGINTERFACECLIENT_SRCS
  debuginterfaceclient.cpp
)
qt5_generate_repc(DEBUGINTERFACECLIENT_SRCS ../debuginterface.rep REPLICA)

add_library(kdstatemachineeditor_debuginterfaceclient SHARED ${DEBUGINTERFACECLIENT_SRCS})
add_library(KDSME::DebugInterfaceClient ALIAS kdstatemachineeditor_debuginterfaceclient)
target_link_libraries(kdstatemachineeditor_debuginterfaceclient
  LINK_PRIVATE
    Qt5::RemoteObjects
  LINK_PUBLIC
    KDSME::Core
    Qt5::Core
)
set_target_properties(kdstatemachineeditor_debuginterfaceclient PROPERTIES
  SOVERSION ${KDSME_SOVERSION}
  VERSION ${KDSME_SOVERSION}
  EXPORT_NAME DebugInterfaceClient
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)
#version libraries on Windows
if(WIN32)
  set(postfix ${KDSME_SOVERSION})
  set(CMAKE_RELEASE_POSTFIX ${postfix})
  set_target_properties(kdstatemachineeditor_debuginterfaceclient PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
  #append 'd' to debug libraries
  string(CONCAT postfix ${postfix} "d")
  set(CMAKE_DEBUG_POSTFIX ${postfix})
  set_target_properties(kdstatemachineeditor_debuginterfaceclient PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
endif()

generate_export_header(kdstatemachineeditor_debuginterfaceclient EXPORT_FILE_NAME kdsme_debuginterfaceclient_export.h BASE_NAME KDSME_DEBUGINTERFACECLIENT)
set(build_iface_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../
)
target_include_directories(kdstatemachineeditor_debuginterfaceclient
  PUBLIC
    "$<BUILD_INTERFACE:${build_iface_dirs}>"
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)
ecm_generate_pri_file(BASE_NAME KDSMEDebugInterfaceClient
                      LIB_NAME kdstatemachineeditor_debuginterfaceclient
                      DEPS ""
                      FILENAME_VAR PRI_FILENAME
                      INCLUDE_INSTALL_DIRS include/ ${INCLUDE_INSTALL_DIR} ${INCLUDE_INSTALL_DIR}/debuginterfaceclient ${INCLUDE_INSTALL_DIR}/debuginterface
)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
install(TARGETS kdstatemachineeditor_debuginterfaceclient EXPORT KDSME_TARGETS ${INSTALL_TARGETS_DEFAULT_ARGS})
if(MSVC)
  install(FILES "$<TARGET_PDB_FILE_DIR:kdstatemachineeditor_debuginterfaceclient>/$<TARGET_PDB_FILE_NAME:kdstatemachineeditor_debuginterfaceclient>" DESTINATION ${LIB_INSTALL_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kdsme_debuginterfaceclient_export.h
    debuginterfaceclient.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/debuginterfaceclient)
