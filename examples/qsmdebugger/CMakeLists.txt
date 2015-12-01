set(SRCS trafficlight.cpp main.cpp)
# Copy the .rep file to your project
qt5_generate_repc(SRCS ../../src/debuginterface/debuginterface.rep REPLICA)
add_executable(qsmdebugger ${SRCS})
target_link_libraries(qsmdebugger
  KDSME::Core
  KDSME::View
  KDSME::DebugInterfaceClient
  KDSME::DebugInterfaceSource
  Qt5::Widgets
  Qt5::RemoteObjects
)
