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

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum InputMode
    {
        PresetsInputMode,
    };

    explicit MainWindow(QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~MainWindow();

    /// When in PresetsInputMode, return the currently selected file name
    QString selectedFile() const;

    void loadPresets(const QString &presetsDir);

public Q_SLOTS:
    void setStateMachine(KDSME::StateMachine *stateMachine);
    void setInputMode(MainWindow::InputMode mode);

    void createNew();

private Q_SLOTS:
    void importFromScxmlFile(const QString &filePath);

    void handlePresetActivated(const QModelIndex &index);

private:
    void setupPresetsView();
    void setupStateMachineView();
    void setupObjectInspector();
    void setupActions();

    Ui::MainWindow *ui;

    QStandardItemModel *m_presetsModel;
    KDSME::TransitionListModel *m_transitionsModel;

    KDSME::StateMachineView *m_stateMachineView;
    QScopedPointer<KDSME::StateMachine, QScopedPointerDeleteLater> m_owningStateMachine;
};

#endif // MAINWINDOW_H
