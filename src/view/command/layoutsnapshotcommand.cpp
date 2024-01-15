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

#include "layoutsnapshotcommand_p.h"

#include "statemachinescene.h"

using namespace KDSME;

LayoutSnapshotCommand::LayoutSnapshotCommand(StateMachineScene *scene, QUndoCommand *parent)
    : Command(nullptr, parent)
    , m_scene(scene)
{
}

LayoutSnapshotCommand::LayoutSnapshotCommand(StateMachineScene *scene, const QString &text, QUndoCommand *parent)
    : Command(text, parent)
    , m_scene(scene)
{
}

void LayoutSnapshotCommand::redo()
{
    // TODO: Implement me
}

void LayoutSnapshotCommand::undo()
{
    // TODO: Implement me
}
