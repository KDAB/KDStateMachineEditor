/*
  modifytransitioncommand.h

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

#ifndef KDSME_COMMAND_MODIFYTRANSITIONCOMMAND_H
#define KDSME_COMMAND_MODIFYTRANSITIONCOMMAND_H

#include "command.h"

#include <QPointer>

namespace KDSME {

class State;
class Transition;

class KDSME_VIEW_EXPORT ModifyTransitionCommand : public Command
{
    Q_OBJECT

public:
    explicit ModifyTransitionCommand(Transition* transition, QUndoCommand* parent = 0);

    virtual int id() const Q_DECL_OVERRIDE { return ModifyTransition; }

    Q_INVOKABLE void setSourceState(KDSME::State* sourceState);
    Q_INVOKABLE void setTargetState(KDSME::State* targetState);

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    void updateText();

    QPointer<Transition> m_transition;

    enum Operation {
        NoOperation,
        SetSourceStateOperation,
        SetTargetStateOperation
    } m_operation;

    QPointer<State> m_sourceState, m_oldSourceState;
    QPointer<State> m_targetState, m_oldTargetState;
};

}

#endif // MODIFYTRANSITIONCOMMAND_H
