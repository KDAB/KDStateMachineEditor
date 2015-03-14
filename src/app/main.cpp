/*
  main.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "mainwindow.h"
#include "scxmlimporter.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <qqmldebug.h>

using namespace KDSME;

namespace
{

QByteArray readFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << file.fileName();
        return QByteArray();
    }
    return file.readAll();
}

}

int main(int argc, char** argv)
{
    // this must be called before the QApplication constructor
    QQmlDebuggingEnabler enabler;

    QApplication app(argc, argv);
    app.setOrganizationName("KDAB");
    app.setApplicationName("kdsme");
    app.setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("State Machine Editor");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "SCXML file"));
    parser.process(app);
    const QStringList args = parser.positionalArguments();
    const QString source = args.value(0);

    StateMachine* stateMachine = nullptr;
    if (!source.isEmpty()) {
        ScxmlImporter parser(readFile(source));
        stateMachine = parser.import();

        if (!stateMachine) {
            qWarning() << "Failed loading" << source << "-" << parser.errorString();
        }
    }

    MainWindow mainWindow;
    mainWindow.resize(1024, 800);
    if (stateMachine) {
        mainWindow.setStateMachine(stateMachine);
    }
    mainWindow.show();

    return app.exec();
}
