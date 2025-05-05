/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "modifydefaultstatecommand_p.h"

#include "state.h"
#include "elementutil.h"

#include "debug.h"

using namespace KDSME;

ModifyDefaultStateCommand::ModifyDefaultStateCommand(HistoryState *state, State *defaultState, QUndoCommand *parent)
    : Command(QString(), parent)
    , m_state(state)
    , m_defaultState(defaultState)
{
    Q_ASSERT(state);
    setText(tr("Changing default state of %1").arg(state->label()));
}

ModifyDefaultStateCommand::~ModifyDefaultStateCommand() = default; // NOLINT(clang-analyzer-cplusplus.NewDelete)

int ModifyDefaultStateCommand::id() const
{
    return Command::ModifyDefaultState;
}

void ModifyDefaultStateCommand::undo()
{
    if (!m_state)
        return;

    m_state->setDefaultState(m_oldDefaultState);
}

void ModifyDefaultStateCommand::redo()
{
    if (!m_state)
        return;

    m_oldDefaultState = m_state->defaultState();
    m_state->setDefaultState(m_defaultState);
}
