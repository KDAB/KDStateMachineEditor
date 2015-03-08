add_executable(test_commands test_commands.cpp)
target_link_libraries(test_commands
  Qt5::Test
  kdstatemachineeditor_view
)
add_test(NAME test_commands COMMAND test_commands)

add_executable(test_statemachineview test_statemachineview.cpp)
target_link_libraries(test_statemachineview
  Qt5::Test
  kdstatemachineeditor_view
)
add_test(NAME test_statemachineview COMMAND test_statemachineview)
