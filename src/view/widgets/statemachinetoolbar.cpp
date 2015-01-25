/*
  statemachinetoolbar.cpp

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

#include "statemachinetoolbar.h"

#include "export/scxmlexporter.h"
#include "export/qmlexporter.h"
#include "statemachineview.h"
#include "statemachinescene.h"

#include <QAction>
#include "debug.h"
#include <QFileDialog>
#include <QMessageBox>

using namespace KDSME;

struct StateMachineToolBar::Private
{
    Private(StateMachineToolBar* q);

    // slots
    void handleExport();

    void init();

    void exportToFile(StateMachine* machine, const QString& fileName);

    StateMachineToolBar* q;
    StateMachineView* m_view;

    QAction* m_exportAction;
};

StateMachineToolBar::Private::Private(StateMachineToolBar* q)
    : q(q)
    , m_view(nullptr)
    , m_exportAction(nullptr)
{
}

StateMachineToolBar::StateMachineToolBar(StateMachineView* view, QWidget* parent)
    : QToolBar(parent)
    , d(new Private(this))
{
    d->m_view = view;

    setWindowTitle(tr("State Machine Tool Bar"));
    d->m_exportAction = new QAction(tr("Export to file"), this);
    d->m_exportAction->setStatusTip("Export current State Machine to file");
    connect(d->m_exportAction, SIGNAL(triggered()), this, SLOT(handleExport()));

    addAction(d->m_exportAction);
}

StateMachineToolBar::~StateMachineToolBar()
{
}

void StateMachineToolBar::Private::handleExport()
{
    StateMachine* stateMachine = m_view->scene()->stateMachine();
    if (!stateMachine) {
        QMessageBox::information(q, QString(), "State machine unavailable");
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(q, tr("Save to File"), QString(), "SCXML/QML files (*.scxml, *.qml)");
    exportToFile(stateMachine, fileName);
}

void StateMachineToolBar::Private::exportToFile(StateMachine* machine, const QString& fileName)
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

#include "moc_statemachinetoolbar.cpp"
