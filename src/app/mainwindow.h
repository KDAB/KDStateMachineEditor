/*
  mainwindow.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_APP_MAINWINDOW_H
#define KDSME_APP_MAINWINDOW_H

#include <QMainWindow>

namespace KDSME {
class StateMachine;
class StateMachineView;
class StateModel;
class TransitionListModel;
class StateMachineScene;
}

QT_BEGIN_NAMESPACE
class QStandardItemModel;

namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum InputMode {
        PresetsInputMode,
    };

    explicit MainWindow(QWidget* parent = nullptr, Qt::WindowFlags f = nullptr);
    ~MainWindow();

    /// When in PresetsInputMode, return the currently selected file name
    QString selectedFile() const;

    void loadPresets(const QString& presetsDir);

public Q_SLOTS:
    void setStateMachine(KDSME::StateMachine* stateMachine);
    void setInputMode(InputMode mode);

    void createNew();

private Q_SLOTS:
    void importFromScxmlFile(const QString& filePath);

    void handlePresetActivated(const QModelIndex& index);

private:
    void setupPresetsView();
    void setupStateMachineView();
    void setupObjectInspector();
    void setupActions();

    Ui::MainWindow* ui;

    QStandardItemModel* m_presetsModel;
    KDSME::TransitionListModel* m_transitionsModel;

    KDSME::StateMachineView* m_stateMachineView;
    QScopedPointer<KDSME::StateMachine, QScopedPointerDeleteLater> m_owningStateMachine;
};

#endif // MAINWINDOW_H
