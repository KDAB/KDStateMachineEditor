Introduction
============
The KDAB State Machine Editor Library is a framework that can be used to
help develop full-featured State Machine Editing graphical user interfaces
and tools.  Output from such applications is in metacode or QML that can
then be used in larger Qt or QtQuick projects.

For a full description of the KDAB State Machine Editor Library, read our
wiki on GitHub, https://github.com/KDAB/KDStateMachineEditor/wiki.

Building and running
================================

Install the dependencies (here: Ubuntu):

    $ sudo apt-get install cmake qt5-default graphviz-dev \
                           qtbase5-private-dev libqt5qml-graphicaleffects

Build:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Start the test app:

    $ ./bin/kdstatemachineeditor

Contributing
============
KDAB will happily accept external contributions, but substantial
contributions will require a signed Copyright Assignment Agreement.
Contact info@kdab.com for more information.

Licensing
=========
The KDAB State Machine Editor Library is (C) 2014-2017 Klarälvdalens Datakonsult AB (KDAB),
and is available under the terms of the LGPL 2.1. See LICENSE.LGPL.txt for license details.

Commercial use is also permitted as described in ReadMe-commercial.txt.

About KDAB
==========
This State Machine Editor Library is supported and maintained by
Klarälvdalens Datakonsult AB (KDAB).

KDAB, the Qt experts, provide consulting and mentoring for developing
Qt applications from scratch and in porting from all popular and legacy
frameworks to Qt. We continue to help develop parts of Qt and are one
of the major contributors to the Qt Project. We can give advanced or
standard trainings anywhere around the globe.

Please visit http://www.kdab.com to meet the people who write code like this.
