/*
  mainwindow.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <config-kdsme.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editcontroller.h"
#include "util/settings.h"
#include "layouter.h"
#include "elementmodel.h"
#include "scxmlparser.h"
#include "element.h"
#include "commandcontroller.h"
#include "statemachinescene.h"
#include "widgets/statemachineview.h"
#include "widgets/statemachinetoolbar.h"

#include <QCoreApplication>
#include <QDebug>
#include <QLayout>
#include <QSettings>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QUndoStack>

using namespace KDSME;

namespace {

enum PresetsModelDataRoles {
    AbsoluteFilePathRole = Qt::UserRole + 1
};

QByteArray readFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << file.fileName();
        return QByteArray();
    }
    return file.readAll();
}

QString presetsLocation()
{
    const QString presetsLocation = qgetenv("KDSME_PRESETS_LOCATION");
    if (!presetsLocation.isEmpty()) {
        return presetsLocation;
    }
    return QCoreApplication::applicationDirPath() + "/../share/" + PACKAGE_NAME + "/data";
}

QString scxmlPresetsLocation()
{
    return presetsLocation() + "/scxml";
}

}

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
    , ui(new Ui::MainWindow)
    , m_presetsModel(new QStandardItemModel(this))
    , m_transitionsModel(new TransitionListModel(this))
    , m_stateMachineView(0)
{
    ui->setupUi(this);

    setupPresetsView();
    setupStateMachineView();
    setupObjectInspector();
    setupActions();

    addToolBar(new StateMachineToolBar(m_stateMachineView));

    setWindowTitle(tr("State Machine Editor"));

    // initialize with an empty state machine
    setStateMachine(nullptr);
}

MainWindow::~MainWindow()
{
    m_stateMachineView->scene()->setStateMachine(nullptr);

    delete ui;
}

void MainWindow::setupStateMachineView()
{
    m_stateMachineView = new StateMachineView;
    m_stateMachineView->editController()->setEditModeEnabled(true);

    ui->sceneBox->layout()->addWidget(m_stateMachineView);
}

void MainWindow::setupPresetsView()
{
    m_presetsModel->clear();

    const QString presetsLocation = ::scxmlPresetsLocation();
    QDir dir(presetsLocation);
    if (!dir.exists()) {
        qWarning() << "Non-existent presets location:" << presetsLocation;
    }

    QStringList files = dir.entryList(QDir::Files);
    foreach (const QString& file, files) {
        if (!file.endsWith(QLatin1String(".scxml")))
            continue;

        QStandardItem* item = new QStandardItem(file);
        item->setData(dir.absoluteFilePath(file), AbsoluteFilePathRole);
        m_presetsModel->appendRow(item);
    }

    m_presetsModel->setHeaderData(0, Qt::Horizontal, tr("Preset"));
    ui->presetsTreeView->setModel(m_presetsModel);
    ui->presetsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->presetsTreeView, SIGNAL(clicked(QModelIndex)), SLOT(handlePresetActivated(QModelIndex)));
}

void MainWindow::setupObjectInspector()
{
    // object inspectors for the state machine object tree
    ui->statesView->setModel(m_stateMachineView->scene()->stateModel());
    ui->transitionsView->setModel(m_transitionsModel);

    ui->undoView->setStack(m_stateMachineView->commandController()->undoStack());
}

void MainWindow::setupActions()
{
    auto action = new QAction(tr("New"), this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(createNew()));
    ui->mainToolBar->addAction(action);
}

void MainWindow::setStateMachine(StateMachine* stateMachine)
{
    if (stateMachine && m_stateMachineView->scene()->stateMachine() == stateMachine)
        return;

    m_stateMachineView->scene()->setStateMachine(nullptr);

    if (!stateMachine) {
        stateMachine = new StateMachine;
        stateMachine->setLabel(tr("New"));
        m_owningStateMachine.reset(stateMachine);
    }

    // update state chart
    m_stateMachineView->scene()->setStateMachine(stateMachine);
    m_stateMachineView->scene()->layout();

    m_transitionsModel->setState(stateMachine);

    auto selectionModel = m_stateMachineView->scene()->selectionModel();
    Q_ASSERT(selectionModel);
    ui->statesView->setSelectionModel(selectionModel);
    ui->propertyEditorWidget->setSelectionModel(selectionModel);
    ui->propertyEditorWidget->setCommandController(m_stateMachineView->commandController());

    ui->statesView->expandAll();
}

void MainWindow::setInputMode(MainWindow::InputMode mode)
{
    if (mode == PresetsInputMode) {
        importFromScxmlFile(selectedFile());
    }
}

void MainWindow::createNew()
{
    setStateMachine(nullptr);
    ui->presetsTreeView->setCurrentIndex(QModelIndex());
}

QString MainWindow::selectedFile() const
{
    const QModelIndex selected = ui->presetsTreeView->currentIndex();
    return selected.data(AbsoluteFilePathRole).toString();
}

void MainWindow::importFromScxmlFile(const QString& filePath)
{
    if (!filePath.isEmpty()) {
        ScxmlParser parser;
        StateMachine* stateMachine = parser.parse(readFile(filePath));
        setStateMachine(stateMachine);
        if (stateMachine) {
            m_owningStateMachine.reset(stateMachine);
        }
    } else {
        setStateMachine(nullptr);
    }

    // update view
    const QModelIndex match = m_presetsModel->match(m_presetsModel->index(0, 0), AbsoluteFilePathRole, QFileInfo(filePath).absoluteFilePath(), 1, Qt::MatchExactly).value(0);
    ui->presetsTreeView->setCurrentIndex(match);
}

void MainWindow::handlePresetActivated(const QModelIndex& index)
{
    const QString fileName = index.data().toString();
    if (fileName.isEmpty())
        return;

    const QString filePath = scxmlPresetsLocation() + '/' + fileName;
    importFromScxmlFile(filePath);
}

#include "ui_mainwindow.h"
