
include("${CMAKE_CURRENT_LIST_DIR}/KDSMEFindDependencyMacro.cmake")

kdsme_find_dependency(Qt5Widgets)
kdsme_find_dependency(Qt5Quick)
kdsme_find_dependency(Qt5QuickWidgets)

include("${CMAKE_CURRENT_LIST_DIR}/KDSMETargets.cmake")
