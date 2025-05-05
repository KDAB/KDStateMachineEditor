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
    const QDir dir(QStringLiteral(":/kdsme/qml/themes"));
    auto entries = dir.entryList();
    std::transform(entries.cbegin(), entries.cend(), entries.begin(), [](const QString &x) {
        return QString(x).remove(QStringLiteral(".qml"));
    });
    return entries;
}

}

struct StateMachineToolBar::Private
{
    Private(StateMachineToolBar *q);

    // slots
    void handleExport();

    void init();

    void exportToFile(StateMachine *machine, const QString &fileName);

    StateMachineToolBar *q;
    StateMachineView *m_view;

    QAction *m_exportAction;
};

StateMachineToolBar::Private::Private(StateMachineToolBar *q)
    : q(q)
    , m_view(nullptr)
    , m_exportAction(nullptr)
{
}

StateMachineToolBar::StateMachineToolBar(StateMachineView *view, QWidget *parent)
    : QToolBar(parent)
    , d(new Private(this))
{
    d->m_view = view;

    setWindowTitle(tr("State Machine Tool Bar"));
    d->m_exportAction = new QAction(tr("Export to File..."), this);
    d->m_exportAction->setObjectName(QStringLiteral("actionExportToFile"));
    d->m_exportAction->setStatusTip(QStringLiteral("Export current state machine to a file."));
    connect(d->m_exportAction, SIGNAL(triggered()), this, SLOT(handleExport())); // clazy:exclude=old-style-connect
    addAction(d->m_exportAction);

    auto *themeSelectionButton = new QToolButton(this);
    themeSelectionButton->setText(tr("Theme"));
    themeSelectionButton->setPopupMode(QToolButton::InstantPopup);
    auto *themeSelectionMenu = new QMenu(themeSelectionButton);
    const auto themes = availableThemeNames();
    for (const QString &themeName : themes) {
        auto action = new QAction(themeName, this);
        action->setObjectName(QStringLiteral("action%1").arg(themeName));
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
        QMessageBox::information(q, QString(), QStringLiteral("State machine unavailable"));
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(q, tr("Save to File"), QString(), tr("SCXML/QML/SVG files (*.scxml, *.qml, *.svg)"));
    exportToFile(stateMachine, fileName);
}

void StateMachineToolBar::Private::exportToFile(StateMachine *machine, const QString &fileName)
{
    if (!machine || fileName.isEmpty())
        return;

    QFile file(fileName);
    const bool success = file.open(QIODevice::WriteOnly);
    if (!success) {
        qCWarning(KDSME_VIEW) << "Failed to open file:" << fileName;
        return;
    }

    const QString suffix = QFileInfo(fileName).suffix();
    AbstractExporter *exporter;
    if (suffix == u"qml") {
        exporter = new QmlExporter(&file);
    } else if (suffix == u"svg") {
        exporter = new SvgExporter(&file);
    } else {
        // fallback
        exporter = new ScxmlExporter(&file);
    }
    exporter->exportMachine(machine);
}

#include "moc_statemachinetoolbar.cpp"
