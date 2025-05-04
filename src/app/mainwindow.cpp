/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-kdsme.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editcontroller.h"
#include "layouter.h"
#include "elementmodel.h"
#include "scxmlimporter.h"
#include "state.h"
#include "transition.h"
#include "commandcontroller.h"
#include "parsehelper.h"
#include "statemachinescene.h"
#include "widgets/statemachineview.h"
#include "widgets/statemachinetoolbar.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLayout>
#include <QSettings>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QUndoStack>

using namespace KDSME;

namespace {

enum PresetsModelDataRoles : quint16
{
    AbsoluteFilePathRole = Qt::UserRole + 1
};

}

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
    , ui(new Ui::MainWindow)
    , m_presetsModel(new QStandardItemModel(this))
    , m_transitionsModel(new TransitionListModel(this))
    , m_stateMachineView(nullptr)
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
    m_stateMachineView->scene()->setRootState(nullptr);

    delete ui;
}

void MainWindow::loadPresets(const QString &presetsDir)
{
    m_presetsModel->clear();

    const QDir dir(presetsDir);
    if (!dir.exists()) {
        qWarning() << "Non-existent presets location:" << presetsDir;
    }

    const QStringList files = dir.entryList(QDir::Files);
    for (const QString &file : files) {
        if (!file.endsWith(u".scxml"))
            continue;

        auto *item = new QStandardItem(file);
        item->setData(dir.absoluteFilePath(file), AbsoluteFilePathRole);
        m_presetsModel->appendRow(item);
    }

    m_presetsModel->setHeaderData(0, Qt::Horizontal, tr("Preset"));
}

void MainWindow::setupStateMachineView()
{
    m_stateMachineView = new StateMachineView;
    m_stateMachineView->editController()->setEditModeEnabled(true);
    setCentralWidget(m_stateMachineView);
}

void MainWindow::setupPresetsView()
{
    ui->presetsTreeView->setModel(m_presetsModel);
    ui->presetsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->presetsTreeView, &QTreeView::clicked, this, &MainWindow::handlePresetActivated);
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
    connect(action, &QAction::triggered, this, &MainWindow::createNew);
    ui->mainToolBar->addAction(action);
}

void MainWindow::setStateMachine(StateMachine *stateMachine)
{
    m_stateMachineView->scene()->setRootState(nullptr);

    if (!stateMachine) {
        stateMachine = new StateMachine;
        stateMachine->setLabel(tr("New"));
        m_owningStateMachine.reset(stateMachine);
    }

    // update state chart
    m_stateMachineView->scene()->setRootState(stateMachine);
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

void MainWindow::importFromScxmlFile(const QString &filePath)
{
    if (!filePath.isEmpty()) {
        ScxmlImporter parser(ParseHelper::readFile(filePath));
        StateMachine *stateMachine = parser.import();
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

void MainWindow::handlePresetActivated(const QModelIndex &index)
{
    const QString filePath = index.data(AbsoluteFilePathRole).toString();
    if (filePath.isEmpty())
        return;

    importFromScxmlFile(filePath);
}
