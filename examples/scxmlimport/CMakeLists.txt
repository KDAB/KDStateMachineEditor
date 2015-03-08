add_executable(scxmlimport main.cpp)
target_link_libraries(scxmlimport
  KDSME::Core
  KDSME::View
  Qt5::Gui
)
