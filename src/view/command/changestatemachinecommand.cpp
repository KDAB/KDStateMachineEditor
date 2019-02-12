/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "changestatemachinecommand_p.h"

#include "elementmodel.h"
#include "state.h"
#include "statemachinescene.h"
#include "transition.h"

#include "debug.h"

using namespace KDSME;

ChangeStateMachineCommand::ChangeStateMachineCommand(KDSME::StateMachineScene* view, QUndoCommand* parent)
    : Command(tr("Change state machine"), parent)
    , m_view(view)
    , m_oldStateMachine(nullptr)
    , m_newStateMachine(nullptr)
{
    Q_ASSERT(view);
}

KDSME::State* ChangeStateMachineCommand::stateMachine() const
{
    return m_newStateMachine;
}

void ChangeStateMachineCommand::setStateMachine(KDSME::State* stateMachine)
{
    if (m_newStateMachine == stateMachine)
        return;
    m_newStateMachine = stateMachine;
    emit stateMachineChanged(stateMachine);
}

void ChangeStateMachineCommand::redo()
{
    qCDebug(KDSME_VIEW) << Q_FUNC_INFO;

    Q_ASSERT(m_view);
    Q_ASSERT(m_view->stateModel());

    m_oldStateMachine = m_view->rootState();

    m_view->stateModel()->setState(m_newStateMachine);
    m_view->setRootState(m_newStateMachine);

    m_view->layout();
}

void ChangeStateMachineCommand::undo()
{
    qCDebug(KDSME_VIEW) << Q_FUNC_INFO;

    Q_ASSERT(m_view);
    Q_ASSERT(m_view->stateModel());

    m_view->stateModel()->setState(m_oldStateMachine);
    m_view->setRootState(m_oldStateMachine);
    m_oldStateMachine = nullptr;

    m_view->layout();
}
