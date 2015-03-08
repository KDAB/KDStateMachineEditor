add_executable(simplestatemachine main.cpp)
target_link_libraries(simplestatemachine
  KDSME::Core
  KDSME::View
  Qt5::Gui
)
