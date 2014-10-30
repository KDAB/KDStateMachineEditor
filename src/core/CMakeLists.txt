set(LIB_SRCS
  export/abstractexporter.cpp
  export/qmlexporter.cpp
  export/scxmlexporter.cpp

  layout/layerwiselayouter.cpp
  layout/layouter.cpp
  layout/layoutimportexport.cpp
  layout/layoutitem.cpp
  layout/layoutitemmodel.cpp
  layout/layoutproperties.cpp
  layout/layoututils.cpp

  model/element.cpp
  model/elementfactory.cpp
  model/elementmodel.cpp
  model/elementutil.cpp
  import/scxmlparser.cpp

  util/objecthelper.cpp
  util/objecttreemodel.cpp
  util/settings.cpp

  view/abstractview.cpp
  view/view.cpp
)

if(Graphviz_FOUND)
  list(APPEND LIB_SRCS
    layout/graphvizlayout/gvutils.cpp
    layout/graphvizlayout/graphvizlayerlayouter.cpp
    layout/graphvizlayout/graphvizlayouter.cpp
    layout/graphvizlayout/graphvizlayouterbackend.cpp
  )
endif()

add_definitions(${GRAPHVIZ_COMPILE_FLAGS})
add_library(kdstatemachineeditor_core SHARED ${LIB_SRCS})
add_library(KDSME::Core ALIAS kdstatemachineeditor_core)
target_link_libraries(kdstatemachineeditor_core
  LINK_PRIVATE
    Qt5::Quick
  LINK_PUBLIC
    Qt5::Core
    Qt5::Gui
)
set_target_properties(kdstatemachineeditor_core PROPERTIES SOVERSION ${KDSME_SOVERSION} VERSION ${KDSME_SOVERSION} EXPORT_NAME Core)
generate_export_header(kdstatemachineeditor_core EXPORT_FILE_NAME kdsme_core_export.h BASE_NAME KDSME_CORE)

if(Graphviz_FOUND)
  target_link_libraries(kdstatemachineeditor_core LINK_PRIVATE Graphviz::gvc)

  if(TARGET Graphviz::cgraph AND Graphviz_VERSION VERSION_GREATER 2.30.0)
    message(STATUS "Enabling use of experimental 'cgraph' library of GraphViz")
    target_link_libraries(kdstatemachineeditor_core LINK_PRIVATE Graphviz::cgraph)
  else()
    target_link_libraries(kdstatemachineeditor_core LINK_PRIVATE Graphviz::graph)
  endif()
  list(APPEND LIB_EXTRA_INCLUDES
    # TODO: Work-around issue in graphviz/types.h header
    # <cgraph.h> is included there, but it should rather be "cgraph.h"
    ${Graphviz_INCLUDE_DIR}/graphviz
  )
  target_compile_definitions(kdstatemachineeditor_core PRIVATE -DGRAPHVIZ_MAJOR_VERSION=${Graphviz_VERSION_MAJOR} -DGRAPHVIZ_MINOR_VERSION=${Graphviz_VERSION_MINOR})
endif()

set(build_iface_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}/common
  ${CMAKE_CURRENT_SOURCE_DIR}/import
  ${CMAKE_CURRENT_SOURCE_DIR}/layout
  ${CMAKE_CURRENT_SOURCE_DIR}/model
  ${CMAKE_CURRENT_SOURCE_DIR}/util
  ${CMAKE_CURRENT_SOURCE_DIR}/view
)
target_include_directories(kdstatemachineeditor_core
  PUBLIC
    "$<BUILD_INTERFACE:${build_iface_dirs}>"
    # FIXME: It is recommended to use a relative path here.
    # Doesn't work with CMake 2.8.11, though, probably needs 2.8.12(?)
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
  PRIVATE
    "${Qt5Qml_PRIVATE_INCLUDE_DIRS}" # qqmlcustomparser_p.h
    ${LIB_EXTRA_INCLUDES}
)

add_custom_command(TARGET kdstatemachineeditor_core POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${PROJECT_BINARY_DIR}/${XDG_DATA_INSTALL_DIR}/data"
)

install(TARGETS kdstatemachineeditor_core EXPORT KDSME_TARGETS ${INSTALL_TARGETS_DEFAULT_ARGS})
install(DIRECTORY data/ DESTINATION ${XDG_DATA_INSTALL_DIR}/data)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kdsme_core_export.h

  export/abstractexporter.h
  export/qmlexporter.h
  export/scxmlexporter.h
  model/element.h
  model/elementmodel.h
  model/elementutil.h
  layout/layouter.h
  layout/layoutimportexport.h
  layout/layoutitem.h
  layout/layoutproperties.h
  view/abstractview.h
  view/view.h
  util/objecttreemodel.h
  util/ringbuffer.h
  util/treewalker.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/core
)

add_subdirectory(tests)
