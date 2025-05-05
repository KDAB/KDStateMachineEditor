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

#include "modifyinitialstatecommand_p.h"

#include "state.h"
#include "elementutil.h"

#include "debug.h"

using namespace KDSME;

ModifyInitialStateCommand::ModifyInitialStateCommand(State *state, State *initialState, QUndoCommand *parent) // NOLINT(clang-analyzer-cplusplus.NewDelete)
    : Command(QString(), parent)
    , m_state(state)
    , m_initialState(initialState)
{
    Q_ASSERT(state);
    setText(tr("Changing initial state of %1").arg(state->label()));
}

ModifyInitialStateCommand::~ModifyInitialStateCommand() = default; // NOLINT(clang-analyzer-cplusplus.NewDelete)

int ModifyInitialStateCommand::id() const
{
    return Command::ModifyInitialState;
}

void ModifyInitialStateCommand::undo()
{
    if (!m_state)
        return;

    ElementUtil::setInitialState(m_state, m_oldInitialState);
}

void ModifyInitialStateCommand::redo()
{
    if (!m_state)
        return;

    m_oldInitialState = ElementUtil::findInitialState(m_state);
    ElementUtil::setInitialState(m_state, m_initialState);
}
