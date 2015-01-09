set(CMAKE_AUTOMOC TRUE)

include(GenerateExportHeader)

set(HAVE_GRAPHVIZ ${Graphviz_FOUND})
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-kdsme.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-kdsme.h
)

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

add_custom_target(copy_data ALL)
add_custom_command(TARGET copy_data
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${PROJECT_BINARY_DIR}/data"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)
add_subdirectory(view)
add_subdirectory(app)
