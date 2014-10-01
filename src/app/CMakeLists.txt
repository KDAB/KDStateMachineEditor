set(KDSME_APPBIN ${CMAKE_PROJECT_NAME})

set(BIN_SRCS
  mainwindow.cpp
  main.cpp
)
set(BIN_UIS
  mainwindow.ui
)

qt5_wrap_ui(BIN_SRCS ${BIN_UIS})

add_executable(${KDSME_APPBIN} WIN32 ${BIN_SRCS})
target_link_libraries(${KDSME_APPBIN}
  Qt5::QuickWidgets
  kdstatemachineeditor_core
  kdstatemachineeditor_view
)
