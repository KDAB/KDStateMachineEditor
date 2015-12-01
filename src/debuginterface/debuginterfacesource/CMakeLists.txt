set(QSMDEBUGINTERFACESOURCE_SRCS
  qsmdebuginterfacesource.cpp
  qsmwatcher.cpp

  ../../core/util/objecthelper.cpp
)
qt5_generate_repc(QSMDEBUGINTERFACESOURCE_SRCS ../debuginterface.rep SOURCE)

add_library(kdstatemachineeditor_debuginterfacesource STATIC ${QSMDEBUGINTERFACESOURCE_SRCS})
#target_include_directories(kdstatemachineeditor_debuginterfacesource PRIVATE ../../core/util)
add_library(KDSME::DebugInterfaceSource ALIAS kdstatemachineeditor_debuginterfacesource)
target_link_libraries(kdstatemachineeditor_debuginterfacesource
  LINK_PRIVATE
    Qt5::RemoteObjects
    kdstatemachineeditor_core
  LINK_PUBLIC
    Qt5::Core
)
set_target_properties(kdstatemachineeditor_debuginterfacesource PROPERTIES
  SOVERSION ${KDSME_SOVERSION}
  VERSION ${KDSME_SOVERSION}
  EXPORT_NAME DebugInterfaceSource
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)
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
                      INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/ ${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}/debuginterfacesource ${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}/debuginterface"
)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
install(TARGETS kdstatemachineeditor_debuginterfacesource EXPORT KDSME_TARGETS ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kdsme_debuginterfacesource_export.h
    qsmdebuginterfacesource.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/debuginterfacesource)
