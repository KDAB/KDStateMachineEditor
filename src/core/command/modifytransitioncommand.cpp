/*
  modifytransitioncommand.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "modifytransitioncommand.h"
#include "model/element.h"

using namespace KDSME;

ModifyTransitionCommand::ModifyTransitionCommand(Transition* transition, QUndoCommand* parent)
    : Command(QString(), parent)
    , m_transition(transition)
    , m_operation(NoOperation)
{
}

void ModifyTransitionCommand::redo()
{
    if (!m_transition)
        return;

    switch (m_operation) {
    case SetSourceStateOperation:
        m_oldSourceState = m_transition->sourceState();
        m_transition->setSourceState(m_sourceState);
        break;
    case SetTargetStateOperation:
        m_oldTargetState = m_transition->targetState();
        m_transition->setTargetState(m_targetState);
        break;
    case NoOperation:
        break;
    }
}

void ModifyTransitionCommand::undo()
{
    if (!m_transition)
        return;

    switch (m_operation) {
    case SetSourceStateOperation:
        m_transition->setSourceState(m_oldSourceState);
        break;
    case SetTargetStateOperation:
        m_transition->setTargetState(m_oldTargetState);
        break;
    case NoOperation:
        break;
    }
}

void ModifyTransitionCommand::setSourceState(State* sourceState)
{
    m_sourceState = sourceState;
    m_operation = SetSourceStateOperation;
    updateText();
}

void ModifyTransitionCommand::setTargetState(State* targetState)
{
    m_targetState = targetState;
    m_operation = SetTargetStateOperation;
    updateText();
}

void ModifyTransitionCommand::updateText()
{
    const QString label = m_transition ? m_transition->label() : tr("<Unknown>");

    switch (m_operation) {
    case SetSourceStateOperation:
        setText(tr("Set source state of '%1'").arg(label));
        break;
    case SetTargetStateOperation:
        setText(tr("Set target state of '%1'").arg(label));
        break;
    case NoOperation:
        break;
    }
}

#include "moc_modifytransitioncommand.cpp"
