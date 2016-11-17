set(SRCS trafficlight.cpp main.cpp)
# Copy the .rep file to your project
qt5_generate_repc(SRCS ../../src/debuginterface/debuginterface.rep REPLICA)
qt5_add_resources(SRCS trafficlight-widgets-static.qrc)
qt5_add_statecharts(SRCS statemachine.scxml)
add_executable(qscxmldebugger ${SRCS})
target_link_libraries(qscxmldebugger
  KDSME::Core
  KDSME::View
  KDSME::DebugInterfaceClient
  KDSME::DebugInterfaceSource
  Qt5::Widgets
  Qt5::RemoteObjects
  Qt5::Scxml
)
