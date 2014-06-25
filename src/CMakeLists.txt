set(CMAKE_AUTOMOC TRUE)

include(GenerateExportHeader)

set(HAVE_GRAPHVIZ ${Graphviz_FOUND})
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-kdsme.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-kdsme.h
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(KDSME_CORELIB ${CMAKE_PROJECT_NAME}_core)
add_subdirectory(core)
set(KDSME_VIEWLIB ${CMAKE_PROJECT_NAME}_view)
add_subdirectory(view)
add_subdirectory(app)
