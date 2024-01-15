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

#include "commandcontroller.h"

#include "command_p.h"
#include "element.h"

#include <QQmlEngine>

using namespace KDSME;

struct CommandController::Private
{
    Private();

    QUndoStack *m_undoStack;
};

CommandController::Private::Private()
    : m_undoStack(nullptr)
{
}

CommandController::CommandController(QUndoStack *undoStack, StateMachineView *parent)
    : AbstractController(parent)
    , d(new Private)
{
    d->m_undoStack = undoStack;
    Q_ASSERT(d->m_undoStack);

    qRegisterMetaType<Command *>();
}

CommandController::~CommandController()
{
}

QUndoStack *CommandController::undoStack() const
{
    return d->m_undoStack;
}

void CommandController::push(KDSME::Command *command)
{
    Q_ASSERT(command);
    QQmlEngine::setObjectOwnership(command, QQmlEngine::CppOwnership); // transfer ownership
    d->m_undoStack->push(command);
}

void CommandController::clear()
{
    d->m_undoStack->clear();
}

#include "moc_commandcontroller.cpp"
