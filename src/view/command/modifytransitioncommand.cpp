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

#include "modifytransitioncommand_p.h"

#include "elementmodel.h"
#include "state.h"
#include "transition.h"

using namespace KDSME;

ModifyTransitionCommand::ModifyTransitionCommand(Transition *transition, StateModel *model, QUndoCommand *parent)
    : Command(model, parent)
    , m_transition(transition)
    , m_operation(NoOperation)
{
}

void ModifyTransitionCommand::redo()
{
    if (!m_transition)
        return;

    switch (m_operation) {
    case SetSourceStateOperation: {
        m_oldSourceState = m_transition->sourceState();
        const ObjectTreeModel::ReparentOperation reparentOperation(model(), m_transition, m_sourceState);
        m_transition->setSourceState(m_sourceState);
    } break;
    case SetTargetStateOperation:
        m_oldTargetState = m_transition->targetState();
        m_transition->setTargetState(m_targetState);
        break;
    case SetShapeOperation:
        m_oldShape = m_transition->shape();
        m_transition->setShape(m_shape);
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
    case SetSourceStateOperation: {
        const ObjectTreeModel::ReparentOperation reparentOperation(model(), m_transition, m_oldSourceState);
        m_transition->setSourceState(m_oldSourceState);
    } break;
    case SetTargetStateOperation:
        m_transition->setTargetState(m_oldTargetState);
        break;
    case SetShapeOperation:
        m_transition->setShape(m_oldShape);
        break;
    case NoOperation:
        break;
    }
}

bool ModifyTransitionCommand::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) {
        return false;
    }

    auto cmd = static_cast<const ModifyTransitionCommand *>(other);
    if (cmd->m_transition != m_transition || cmd->m_operation != m_operation) {
        return false;
    }

    m_sourceState = cmd->m_sourceState;
    m_targetState = cmd->m_targetState;
    m_shape = cmd->m_shape;
    return true;
}

void ModifyTransitionCommand::setSourceState(State *sourceState)
{
    m_sourceState = sourceState;
    m_operation = SetSourceStateOperation;
    updateText();
}

void ModifyTransitionCommand::setTargetState(State *targetState)
{
    m_targetState = targetState; // NOLINT(clang-analyzer-cplusplus.NewDelete)
    m_operation = SetTargetStateOperation;
    updateText();
}

void ModifyTransitionCommand::setShape(const QPainterPath &shape)
{
    m_shape = shape;
    m_operation = SetShapeOperation;
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
    case SetShapeOperation:
        setText(tr("Modify path of '%1'").arg(label));
        break;
    case NoOperation:
        break;
    }
}
