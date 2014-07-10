set(LIB_SRCS
  quick/qopengl2pexvertexarray.cpp # Copy because QtGui doesn't export all symbols
  quick/quickmaskedmousearea.cpp
  quick/quickpainterpath.cpp
  quick/quickpen.cpp
  quick/quickprimitiveitem.cpp
  quick/quickkdsmeglobal.cpp

  widgets/codeeditor.cpp
  widgets/propertyeditor.cpp
  widgets/statemachinetoolbar.cpp
  widgets/statemachineview.cpp
  widgets/statemachinepalettewidget.cpp
)

set(LIB_UIS
  widgets/statepropertyeditor.ui
  widgets/transitionpropertyeditor.ui
)

qt5_wrap_ui(LIB_SRCS ${LIB_UIS})

set(LIB_QRCS kdsme.qrc)
qt5_add_resources(LIB_SRCS ${LIB_QRCS})

# Make the files show up in Qt Creator
set(QML_FILES
  qml/LayoutItemLoader.qml
  qml/StateMachineDebugToolBar.qml
  qml/util/ChannelizedDropArea.qml
  qml/util/DragPointGroup.qml
  qml/util/SquareButton.qml
  qml/util/DragPoint.qml
  qml/util/BirdEyeView.qml
  qml/util/MoveArea.qml
  qml/util/DragPointDelegate.qml
  qml/util/ToolTip.qml
  qml/util/ArrowHead.qml
  qml/StateMachineView.qml
  qml/StateMachineViewToolBar.qml
  qml/uml/UmlCompositeStateDelegate.qml
  qml/uml/UmlElement.qml
  qml/uml/UmlTransition.qml
  qml/uml/UmlPseudoState.qml
  qml/uml/UmlFinalState.qml
  qml/uml/UmlHistoryState.qml
  qml/uml/UmlStateMachine.qml
  qml/uml/UmlState.qml
  qml/StateMachineScene.qml
)

add_library(${KDSME_VIEWLIB} SHARED ${LIB_SRCS} ${LIB_QRCS} ${QML_FILES})
target_link_libraries(${KDSME_VIEWLIB}
  LINK_PRIVATE
    ${KDSME_CORELIB}
  LINK_PUBLIC
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::Widgets
)
set_target_properties(${KDSME_VIEWLIB} PROPERTIES SOVERSION ${KDSME_SOVERSION} VERSION ${KDSME_SOVERSION})
generate_export_header(${KDSME_VIEWLIB} EXPORT_FILE_NAME kdsme_view_export.h BASE_NAME KDSME_VIEW)
target_include_directories(${KDSME_VIEWLIB}
  # FIXME: It is recommended to use a relative path here.
  # Doesn't work with CMake 2.8.11, though, probably needs 2.8.12(?)
  INTERFACE "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
  PRIVATE ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

install(TARGETS ${KDSME_VIEWLIB} EXPORT KDSME_TARGETS ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kdsme_view_export.h

  widgets/propertyeditor.h
  widgets/statemachinepalettewidget.h
  widgets/statemachineview.h
  widgets/statemachinetoolbar.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/view
)
