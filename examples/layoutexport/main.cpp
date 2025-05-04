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

#include <scxmlimporter.h>
#include <state.h>
#include <layoutimportexport.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QScopedPointer>
#include <QTextStream>

using namespace KDSME;

/**
 * @brief Loading a SCXML file, layouting it, and print out just (reusable) layout information
 */
int main()
{
    QTextStream qErr(stderr);
    QTextStream qOut(stdout);

    const QString fileName = QStringLiteral(TEST_DATA_DIR) + u"/scxml/trafficlight_alternative.scxml";
    QFile file(fileName);
    if (!file.exists()) {
        qErr << "File does not exist:" << file.fileName();
        return 1;
    }

    Q_ASSERT(file.open(QIODevice::ReadOnly));
    ScxmlImporter parser(file.readAll());
    const QScopedPointer<StateMachine> machine(parser.import());
    Q_ASSERT(machine);

    //! [Export the layout]
    const QJsonDocument layout(LayoutImportExport::exportLayout(machine.data()));
    qOut << "Layout in JSON format";
    qOut << layout.toJson();
    //! [Export the layout]

    return 0;
}
