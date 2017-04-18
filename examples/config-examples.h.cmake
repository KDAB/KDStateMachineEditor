#ifndef CONFIG_EXAMPLES_H
#define CONFIG_EXAMPLES_H

#include <qglobal.h>

#cmakedefine TEST_DATA_DIR "${TEST_DATA_DIR}"

#cmakedefine Qt5RemoteObjects_VERSION QT_VERSION_CHECK(${Qt5RemoteObjects_VERSION_MAJOR}, ${Qt5RemoteObjects_VERSION_MINOR}, ${Qt5RemoteObjects_VERSION_PATCH})

#endif
