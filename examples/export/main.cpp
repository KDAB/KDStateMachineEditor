/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "config-examples.h"

#include <state.h>
#include <qmlexporter.h>
#include <scxmlexporter.h>

#include <QTextStream>

using namespace KDSME;

namespace {

void doOutput(QTextStream &qOut, const QByteArray &output)
{
    qOut << output << Qt::endl;
}

} // namespace

/**
 * @brief Instantiate a state chart by hand and then export it to different formats
 */
int main()
{
    QTextStream qErr(stderr);
    QTextStream qOut(stdout);

    //! [Construct the state machine]
    StateMachine machine;
    machine.setLabel(QStringLiteral("root"));
    State s1(&machine);
    s1.setLabel(QStringLiteral("s1"));
    State s2(&machine);
    s2.setLabel(QStringLiteral("s2"));
    s1.addSignalTransition(&s2);
    //! [Construct the state machine]

    //! [Export to QML]
    QByteArray qmlOutput;
    QmlExporter qmlExporter(&qmlOutput);
    if (!qmlExporter.exportMachine(&machine)) {
        qErr << "Failed to export to QML:" << qmlExporter.errorString();
        return 1;
    }

    doOutput(qOut, "=== Output [QML] ===");
    doOutput(qOut, qmlOutput);
    doOutput(qOut, {});

    //! [Export to QML]

    //! [Export to SCXML]
    QByteArray scxmlOutput;
    ScxmlExporter scxmlExporter(&scxmlOutput);
    if (!scxmlExporter.exportMachine(&machine)) {
        qErr << "Failed to export to SCXML:" << scxmlExporter.errorString();
        return 1;
    }

    doOutput(qOut, "=== Output [SCXML] ===");
    doOutput(qOut, scxmlOutput);
    doOutput(qOut, {});
    //! [Export to SCXML]
}
