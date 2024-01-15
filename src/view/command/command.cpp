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

#include "command_p.h"

#include "elementmodel.h"

using namespace KDSME;

Command::Command(KDSME::StateModel *model, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_model(model)
{
}

Command::Command(const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent)
    , m_model(nullptr)
{
}

KDSME::StateModel *Command::model() const
{
    return m_model;
}
