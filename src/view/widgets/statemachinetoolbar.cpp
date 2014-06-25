/*
  statemachinetoolbar.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "statemachinetoolbar.h"

#include "export/scxmlexporter.h"
#include "export/qmlexporter.h"
#include "statemachineview.h"
#include "view.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

using namespace KDSME;

StateMachineToolBar::StateMachineToolBar(StateMachineView* view, QWidget* parent)
    : QToolBar(parent)
    , m_view(view)
{
    setWindowTitle(tr("State Machine Tool Bar"));
    m_exportAction = new QAction(tr("Export to file"), this);
    m_exportAction->setStatusTip("Export current State Machine to file");
    connect(m_exportAction, &QAction::triggered, this, &StateMachineToolBar::handleExport);

    setupToolBar();
}

void StateMachineToolBar::setupToolBar()
{
    addAction(m_exportAction);
}

void StateMachineToolBar::handleExport()
{
    StateMachine* stateMachine = m_view->view()->stateMachine();
    if (!stateMachine) {
        QMessageBox::information(this, QString(), "State machine unavailable");
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save to File"), QString(), "SCXML/QML files (*.scxml, *.qml)");
    exportToFile(stateMachine, fileName);
}

void StateMachineToolBar::exportToFile(StateMachine* machine, const QString& fileName)
{
    if (!machine || fileName.isEmpty())
        return;

    QFile file(fileName);
    bool success = file.open(QIODevice::WriteOnly);
    if (!success) {
        qWarning() << "Failed to open file:" << fileName;
        return;
    }

    const QString suffix = QFileInfo(fileName).suffix();
    AbstractExporter* exporter;
    if (suffix == "qml") {
        exporter = new QmlExporter(&file);
    } else {
        // fallback
        exporter = new ScxmlExporter(&file);
    }
    exporter->exportMachine(machine);
}
