

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-test.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-test.h
)

# QtXmlPatterns fails badly on OSX -- see https://bugreports.qt-project.org/browse/QTBUG-40818
if(Qt5XmlPatterns_FOUND AND NOT APPLE)
  add_definitions(-DUSE_QT_XMLPATTERNS_LIB)
  list(APPEND KDSME_TESTHELPER_EXTRA_LIBS ${Qt5XmlPatterns_LIBRARIES})
endif()

add_library(kdsme_testhelper STATIC
  parsehelper.cpp
  util.cpp
)
target_link_libraries(kdsme_testhelper
  Qt5::Test
  kdstatemachineeditor_core
)

if(Graphviz_FOUND)
  add_executable(test_layouter test_layouter.cpp)
  target_link_libraries(test_layouter
    Qt5::Gui
    kdsme_testhelper
  )
  add_test(NAME test_layouter COMMAND test_layouter)
endif()

add_executable(test_layoutitem test_layoutitem.cpp)
target_link_libraries(test_layoutitem
  kdsme_testhelper
)
add_test(NAME test_layoutitem COMMAND test_layoutitem)

add_executable(test_models test_models.cpp)
target_link_libraries(test_models
  Qt5::Gui
  Qt5::Test
  kdstatemachineeditor_core
)
add_test(NAME test_models COMMAND test_models)

add_executable(test_qmlexport test_qmlexport.cpp)
target_link_libraries(test_qmlexport
  kdsme_testhelper
)
add_test(NAME test_qmlexport COMMAND test_qmlexport)

add_executable(test_scxmlimport test_scxmlimport.cpp)
target_link_libraries(test_scxmlimport
  kdsme_testhelper
)
add_test(NAME test_scxmlimport COMMAND test_scxmlimport)

add_executable(test_scxmlexport test_scxmlexport.cpp)
target_link_libraries(test_scxmlexport
  Qt5::Test
  ${KDSME_TESTHELPER_EXTRA_LIBS}
  kdstatemachineeditor_core
)
add_test(NAME test_scxmlexport COMMAND test_scxmlexport)

add_executable(test_statemachine test_statemachine.cpp)
target_link_libraries(test_statemachine
  kdsme_testhelper
)
add_test(NAME test_statemachine COMMAND test_statemachine)

add_executable(test_util test_util.cpp)
target_link_libraries(test_util
  Qt5::Gui
  kdsme_testhelper
)
add_test(NAME test_util COMMAND test_util)

add_executable(test_layoutinformation test_layoutinformation.cpp)
target_link_libraries(test_layoutinformation
  Qt5::Gui
  Qt5::Test
  kdstatemachineeditor_core
)
add_test(NAME test_layoutinformation COMMAND test_layoutinformation)
