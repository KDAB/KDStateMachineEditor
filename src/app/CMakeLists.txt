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
  ${KDSME_CORELIB}
  ${KDSME_VIEWLIB}
)
install(TARGETS ${KDSME_APPBIN} ${INSTALL_TARGETS_DEFAULT_ARGS})

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP ON)
include(InstallRequiredSystemLibraries)
set(KDSME_EXECUTABLE ${BIN_INSTALL_DIR}/${KDSME_APPBIN}${CMAKE_EXECUTABLE_SUFFIX})
if(DEPLOY_DEPENDENCIES)
  if(APPLE OR WIN32)
    include(DeployQt5)
    install_qt5_executable("${KDSME_EXECUTABLE}" "" "" "${Graphviz_ROOT}/bin")
  endif()
endif()
