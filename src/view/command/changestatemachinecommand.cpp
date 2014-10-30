/*
  changestatemachinecommand.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "changestatemachinecommand.h"

#include "element.h"
#include "elementmodel.h"
#include "view.h"

#include <QDebug>

using namespace KDSME;

ChangeStateMachineCommand::ChangeStateMachineCommand(KDSME::View* view, QUndoCommand* parent)
    : Command(tr("Change Statemachine"), parent)
    , m_view(view)
    , m_oldStateMachine(0)
    , m_newStateMachine(0)
{
    Q_ASSERT(view);
}

KDSME::StateMachine* ChangeStateMachineCommand::stateMachine() const
{
    return m_newStateMachine;
}

void ChangeStateMachineCommand::setStateMachine(KDSME::StateMachine* statemachine)
{
    if (m_newStateMachine == statemachine)
        return;
    m_newStateMachine = statemachine;
    emit stateMachineChanged(statemachine);
}

void ChangeStateMachineCommand::redo()
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(m_view);
    Q_ASSERT(m_view->stateModel());

    m_oldStateMachine = m_view->stateMachine();

    m_view->stateModel()->setState(m_newStateMachine);
    m_view->setStateMachine(m_newStateMachine);

    m_view->layout();
}

void ChangeStateMachineCommand::undo()
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(m_view);
    Q_ASSERT(m_view->stateModel());

    m_view->stateModel()->setState(m_oldStateMachine);
    m_view->setStateMachine(m_oldStateMachine);
    m_oldStateMachine = 0;

    m_view->layout();
}

#include "moc_changestatemachinecommand.cpp"
