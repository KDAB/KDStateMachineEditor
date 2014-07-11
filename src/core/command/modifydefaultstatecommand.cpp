/*
  ModifyDefaultStateCommand.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "modifydefaultstatecommand.h"

#include "element.h"
#include "elementutil.h"

#include <QDebug>

using namespace KDSME;

ModifyDefaultStateCommand::ModifyDefaultStateCommand(HistoryState* state, State* defaultState, QUndoCommand* parent):
    Command(QString(), parent),
    m_state(state),
    m_defaultState(defaultState)
{
    Q_ASSERT(state);
    setText(tr("Changing default state of %1").arg(state->label()));
}

ModifyDefaultStateCommand::~ModifyDefaultStateCommand()
{
}

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
