#ifndef KDSME_CONFIG_H
#define KDSME_CONFIG_H

#include <qglobal.h>

/* Define to the full name of this package. */
#define PACKAGE_NAME "${CMAKE_PROJECT_NAME}"

#cmakedefine01 HAVE_GRAPHVIZ

#cmakedefine01 WITH_INTERNAL_GRAPHVIZ

#cmakedefine Qt5RemoteObjects_VERSION QT_VERSION_CHECK(${Qt5RemoteObjects_VERSION_MAJOR}, ${Qt5RemoteObjects_VERSION_MINOR}, ${Qt5RemoteObjects_VERSION_PATCH})

#endif
