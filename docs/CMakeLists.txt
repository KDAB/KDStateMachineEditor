find_program(QHELPGEN_EXECUTABLE qhelpgenerator
  HINTS ${QT_INSTALL_BINS}
)

if(NOT QHELPGEN_EXECUTABLE)
  message(STATUS "Unable to build API documentation in qch format.")
  message(STATUS "qhelpgenerator exectuable: ${QHELPGEN_EXECUTABLE}")
endif()

# Doxygen
find_package(Doxygen)
set_package_properties(Doxygen PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "API Documentation system"
  URL "http://www.doxygen.org"
  PURPOSE "Needed to build the API documentation."
)
if(DOXYGEN_FOUND AND QHELPGEN_EXECUTABLE)
  add_subdirectory(api)
endif()
