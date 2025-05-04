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
#include <statemachinescene.h>
#include <statemachineview.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QScopedPointer>

using namespace KDSME;

/**
 * @brief Loading a SCXML file from disk and displaying it
 */
int main(int argc, char **argv)
{
    const QApplication app(argc, argv);

    const QString fileName = QStringLiteral(TEST_DATA_DIR "/scxml/microwave.scxml");
    QFile file(fileName);
    if (!file.exists()) {
        qWarning() << "File does not exist:" << file.fileName();
        return 1;
    }

    Q_ASSERT(file.open(QIODevice::ReadOnly));
    ScxmlImporter parser(file.readAll());
    const QScopedPointer<StateMachine> machine(parser.import());
    Q_ASSERT(machine);

    StateMachineView view;
    view.scene()->setRootState(machine.data());
    view.scene()->layout();
    view.resize(800, 600);
    view.show();

    QApplication::exec();
}
