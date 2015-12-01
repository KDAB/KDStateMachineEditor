add_subdirectory(debuginterfacesource)
add_subdirectory(debuginterfaceclient)

install(FILES
  debuginterface.rep
  debuginterface_types.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/debuginterface
)
