/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sebastian Sauer <sebastian.sauer@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_COMMAND_CHANGESTATEMACHINECOMMAND_P_H
#define KDSME_COMMAND_CHANGESTATEMACHINECOMMAND_P_H

#include "command_p.h"

#include "state.h"

#include <QPointer>

namespace KDSME {

class StateMachineScene;
class StateMachine;

/**
 * @brief Changes the state machine of a @ref KDSME::StateMachineScene and records the old state machine the scene was showing.
 *
 * On undo, the old state machine is restored.
 */
class KDSME_VIEW_EXPORT ChangeStateMachineCommand : public Command
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State *stateMachine READ stateMachine WRITE setStateMachine NOTIFY stateMachineChanged)

public:
    explicit ChangeStateMachineCommand(KDSME::StateMachineScene *view, QUndoCommand *parent = nullptr);

    int id() const override
    {
        return ChangeStateMachine;
    }

    KDSME::State *stateMachine() const;
    void setStateMachine(KDSME::State *stateMachine);

    void redo() override;
    void undo() override;

Q_SIGNALS:
    void stateMachineChanged(KDSME::State *statemachine);

private:
    QPointer<StateMachineScene> m_view;
    QPointer<State> m_oldStateMachine, m_newStateMachine;
};

}

#endif // REPARENTELEMENTCOMMAND_P_H
