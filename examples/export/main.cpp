/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  This file may be distributed and/or modified under the terms of the
  GNU Lesser General Public License version 2.1 as published by the
  Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.

  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Contact info@kdab.com if any conditions of this licensing are not
  clear to you.
*/

#include "config-examples.h"

#include <state.h>
#include <qmlexporter.h>
#include <scxmlexporter.h>

#include <QTextStream>

using namespace KDSME;

/**
 * @brief Instantiate a state chart by hand and then export it to different formats
 */
int main()
{
    QTextStream qErr(stderr);
    QTextStream qOut(stdout);

    //! [Construct the state machine]
    StateMachine machine;
    machine.setLabel("root");
    State s1(&machine);
    s1.setLabel("s1");
    State s2(&machine);
    s2.setLabel("s2");
    s1.addSignalTransition(&s2);
    //! [Construct the state machine]

    //! [Export to QML]
    QByteArray qmlOutput;
    QmlExporter qmlExporter(&qmlOutput);
    if (!qmlExporter.exportMachine(&machine)) {
        qErr << "Failed to export to QML:" << qmlExporter.errorString();
        return 1;
    }

    qOut << "=== Output [QML] ===" << endl;
    qOut << qmlOutput << endl;
    qOut << endl;
    //! [Export to QML]

    //! [Export to SCXML]
    QByteArray scxmlOutput;
    ScxmlExporter scxmlExporter(&scxmlOutput);
    if (!scxmlExporter.exportMachine(&machine)) {
        qErr << "Failed to export to SCXML:" << scxmlExporter.errorString();
        return 1;
    }

    qOut << "=== Output [SCXML] ===" << endl;
    qOut << scxmlOutput << endl;
    qOut << endl;
    //! [Export to SCXML]
}
