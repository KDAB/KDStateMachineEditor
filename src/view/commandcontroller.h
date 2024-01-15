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

#ifndef KDSME_COMMANDCONTROLLER_H
#define KDSME_COMMANDCONTROLLER_H

#include "kdsme_view_export.h"

#include "abstractcontroller.h"

QT_BEGIN_NAMESPACE
class QUndoCommand;
class QUndoStack;
QT_END_NAMESPACE

namespace KDSME {
class Command;
class State;
class StateModel;

class KDSME_VIEW_EXPORT CommandController : public AbstractController
{
    Q_OBJECT
    Q_PROPERTY(QUndoStack *undoStack READ undoStack CONSTANT)

public:
    explicit CommandController(QUndoStack *undoStack, StateMachineView *parent);
    ~CommandController();

    Q_INVOKABLE void push(KDSME::Command *command);

    void clear();

    QUndoStack *undoStack() const;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::CommandController *)

#endif // COMMANDCONTROLLER_H
