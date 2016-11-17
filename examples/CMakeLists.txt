include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-examples.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-examples.h
)

add_subdirectory(layoutexport)
add_subdirectory(export)
if(Qt5RemoteObjects_FOUND)
    add_subdirectory(qsmdebugger)
    if(Qt5Scxml_FOUND)
        add_subdirectory(qscxmldebugger)
    endif()
endif()
add_subdirectory(scxmlimport)
add_subdirectory(simplestatemachine)
