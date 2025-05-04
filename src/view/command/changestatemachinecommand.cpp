/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sebastian Sauer <sebastian.sauer@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "changestatemachinecommand_p.h"

#include "elementmodel.h"
#include "statemachinescene.h"
#include "transition.h"

#include "debug.h"

using namespace KDSME;

ChangeStateMachineCommand::ChangeStateMachineCommand(KDSME::StateMachineScene *view, QUndoCommand *parent)
    : Command(tr("Change state machine"), parent)
    , m_view(view)
    , m_oldStateMachine(nullptr)
    , m_newStateMachine(nullptr)
{
    Q_ASSERT(view);
}

KDSME::State *ChangeStateMachineCommand::stateMachine() const
{
    return m_newStateMachine;
}

void ChangeStateMachineCommand::setStateMachine(KDSME::State *stateMachine)
{
    if (m_newStateMachine != stateMachine) { // NOLINT(clang-analyzer-cplusplus.NewDelete)
        m_newStateMachine = stateMachine;
        Q_EMIT stateMachineChanged(stateMachine);
    }
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
