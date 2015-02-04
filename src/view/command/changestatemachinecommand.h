/*
  changestatemachinecommand.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Sebastian Sauer <sebastian.sauer@kdab.com>

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

#ifndef KDSME_COMMAND_CHANGESTATEMACHINECOMMAND_H
#define KDSME_COMMAND_CHANGESTATEMACHINECOMMAND_H

#include "command.h"

#include <QPointer>

namespace KDSME {

class StateMachineScene;
class StateMachine;
class State;

class KDSME_VIEW_EXPORT ChangeStateMachineCommand : public Command
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State* stateMachine READ stateMachine WRITE setStateMachine NOTIFY stateMachineChanged)

public:
    explicit ChangeStateMachineCommand(KDSME::StateMachineScene* view, QUndoCommand* parent = 0);

    virtual int id() const { return ChangeStateMachine; }

    KDSME::State* stateMachine() const;
    void setStateMachine(KDSME::State* statemachine);

    virtual void redo() override;
    virtual void undo() override;

signals:
    void stateMachineChanged(KDSME::State* statemachine);

private:
    QPointer<StateMachineScene> m_view;
    QPointer<State> m_oldStateMachine, m_newStateMachine;
};

}

#endif // REPARENTELEMENTCOMMAND_H
