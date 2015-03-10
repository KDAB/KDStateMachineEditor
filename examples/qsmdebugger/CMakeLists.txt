add_executable(qsmdebugger trafficlight.cpp main.cpp)
target_link_libraries(qsmdebugger
  KDSME::Core
  KDSME::View
  Qt5::Widgets
  Qt5::RemoteObjects
)
