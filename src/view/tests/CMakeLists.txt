ecm_add_test(test_commands.cpp
  LINK_LIBRARIES
    Qt5::Test
    kdstatemachineeditor_view
)

ecm_add_test(test_statemachineview.cpp
  LINK_LIBRARIES
    Qt5::Test
    kdstatemachineeditor_view
)
