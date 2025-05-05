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

#ifndef KDSME_WIDGETS_STATEMACHINEVIEW_H
#define KDSME_WIDGETS_STATEMACHINEVIEW_H

#include "kdsme_view_export.h"

#include <QQuickWidget>

namespace KDSME {

class Command;
class EditController;
class CommandController;
class StateMachineScene;
class StateMachine;
class Element;

/**
 * @brief Widget for displaying a @ref KDSME::StateMachine in a Qt Quick based view
 *
 * Simple example building up and display a state machine in a view:
 *
 * @snippet simplestatemachine/main.cpp Display the state machine
 */
class KDSME_VIEW_EXPORT StateMachineView : public QQuickWidget
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateMachineScene *scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(KDSME::CommandController *commandController READ commandController CONSTANT)
    Q_PROPERTY(KDSME::EditController *editController READ editController CONSTANT)

    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)

public:
    explicit StateMachineView(QWidget *parent = nullptr);
    ~StateMachineView();

    StateMachineScene *scene() const;

    CommandController *commandController() const;
    EditController *editController() const;

    QString defaultThemeName() const;
    QString themeName() const;
    void setThemeName(const QString &themeName);

    Q_INVOKABLE void sendCommand(KDSME::Command *cmd) const;

    Q_INVOKABLE void fitInView();

    Q_INVOKABLE void changeStateMachine(KDSME::StateMachine *stateMachine);
    Q_INVOKABLE void deleteElement(KDSME::Element *element);

protected:
    QQuickItem *viewPortObject() const;
    QQuickItem *sceneObject() const;

Q_SIGNALS:
    void sceneChanged(KDSME::StateMachineScene *scene);
    void themeNameChanged(const QString &themeName);

private:
    struct Private;
    QScopedPointer<Private> d;

    void setScene(StateMachineScene *scene);

    Q_PRIVATE_SLOT(d, void onStateMachineChanged(KDSME::StateMachine *))
};

}

/**
 * @example simplestatemachine/main.cpp
 *
 * This examples shows how to construct a simple @ref KDSME::StateMachine and show it in a @ref KDSME::StateMachineView
 */

#endif // STARTCHARTVIEW_H
