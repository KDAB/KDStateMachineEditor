/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2015-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
 * @brief Loading a SCXML file, layouting it, and print out just (re-usable) layout information
 */
int main()
{
    QTextStream qErr(stderr);
    QTextStream qOut(stdout);

    const QString fileName = TEST_DATA_DIR "/scxml/trafficlight_alternative.scxml";
    QFile file(fileName);
    if (!file.exists()) {
        qErr << "File does not exist:" << file.fileName();
        return 1;
    }

    Q_ASSERT(file.open(QIODevice::ReadOnly));
    ScxmlImporter parser(file.readAll());
    QScopedPointer<StateMachine> machine(parser.import());
    Q_ASSERT(machine);

    //! [Export the layout]
    const QJsonDocument layout(LayoutImportExport::exportLayout(machine.data()));
    qOut << "Layout in JSON format";
    qOut << layout.toJson();
    //! [Export the layout]

    return 0;
}
