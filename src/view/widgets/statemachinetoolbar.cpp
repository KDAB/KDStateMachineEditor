/*
  statemachinetoolbar.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "debug.h"
#include "export/scxmlexporter.h"
#include "export/qmlexporter.h"
#include "export/svgexporter.h"
#include "state.h"
#include "statemachineview.h"
#include "statemachinescene.h"

#include <QAction>
#include <QDir>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QWidgetAction>

#include <algorithm>

using namespace KDSME;

namespace {

QStringList availableThemeNames()
{
    QDir dir(":/kdsme/qml/themes");
    auto entries = dir.entryList();
    std::transform(entries.cbegin(), entries.cend(), entries.begin(), [](const QString& x) {
        return QString(x).remove(".qml");
    });
    return entries;
}

}

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
    d->m_exportAction = new QAction(tr("Export to File..."), this);
    d->m_exportAction->setStatusTip("Export current state machine to a file.");
    connect(d->m_exportAction, SIGNAL(triggered()), this, SLOT(handleExport()));
    addAction(d->m_exportAction);

    QToolButton* themeSelectionButton = new QToolButton(this);
    themeSelectionButton->setText(tr("Theme"));
    themeSelectionButton->setPopupMode(QToolButton::InstantPopup);
    QMenu* themeSelectionMenu = new QMenu(themeSelectionButton);
    foreach (const QString& themeName, availableThemeNames()) {
        auto action = new QAction(themeName, this);
        connect(action, &QAction::triggered, this, [this, themeName]() {
            d->m_view->setThemeName(themeName);
        });
        themeSelectionMenu->addAction(action);
    }
    themeSelectionButton->setMenu(themeSelectionMenu);
    addWidget(themeSelectionButton);
}

StateMachineToolBar::~StateMachineToolBar()
{
}

void StateMachineToolBar::Private::handleExport()
{
    auto stateMachine = m_view->scene()->rootState()->machine();
    if (!stateMachine) {
        QMessageBox::information(q, QString(), "State machine unavailable");
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(q, tr("Save to File"), QString(), tr("SCXML/QML/SVG files (*.scxml, *.qml, *.svg)"));
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
    } else if (suffix == "svg") {
        exporter = new SvgExporter(&file);
    } else {
        // fallback
        exporter = new ScxmlExporter(&file);
    }
    exporter->exportMachine(machine);
}

#include "moc_statemachinetoolbar.cpp"
