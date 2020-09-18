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

Get Involved
============
KDAB will happily accept external contributions, but substantial
contributions will require a signed Copyright Assignment Agreement
(see docs/KDStateMachineEditor-CopyrightAssignmentForm.docx).

Contact info@kdab.com for more information.

Please submit your contributions or issue reports from our GitHub space at
https://github.com/KDAB/KDStateMachineEditor

Licensing
=========
The KDAB State Machine Editor Library is (C) 2014-2020 Klarälvdalens Datakonsult AB (KDAB), and
is available under the terms of the LGPL 2.1. See LICENSES/LGPL-2.1-only.txt for license details.

Commercial licensing terms are available in the included file
LICENSES/LicenseRef-KDAB-KDStateMachineEditor.txt.

For terms of redistribution, refer to the corresponding license agreement.

About KDAB
==========
This State Machine Editor Library is supported and maintained by
Klarälvdalens Datakonsult AB (KDAB).

The KDAB Group is the global No.1 software consultancy for Qt, C++ and
OpenGL applications across desktop, embedded and mobile platforms.

The KDAB Group provides consulting and mentoring for developing Qt applications
from scratch and in porting from all popular and legacy frameworks to Qt.
We continue to help develop parts of Qt and are one of the major contributors
to the Qt Project. We can give advanced or standard trainings anywhere
around the globe on Qt as well as C++, OpenGL, 3D and more.

Please visit https://www.kdab.com to meet the people who write code like this.
