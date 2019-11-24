set(HAVE_GRAPHVIZ ${GRAPHVIZ_FOUND})
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-kdsme.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-kdsme.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)
add_subdirectory(view)

if(Qt5RemoteObjects_FOUND)
  add_subdirectory(debuginterface)
endif()

if(BUILD_TESTS)
  add_subdirectory(app)
endif()
