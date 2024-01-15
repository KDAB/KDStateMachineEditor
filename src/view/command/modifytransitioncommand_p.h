/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_COMMAND_MODIFYTRANSITIONCOMMAND_P_H
#define KDSME_COMMAND_MODIFYTRANSITIONCOMMAND_P_H

#include "command_p.h"

#include <QPainterPath>
#include <QPointer>

namespace KDSME {

class State;
class Transition;

/**
 * @brief Command for modifying properties of a @ref KDSME::Transition
 *
 * A command can only set one property at the time.
 * On undo, the previous value of the particular property is restored.
 */
class KDSME_VIEW_EXPORT ModifyTransitionCommand : public Command
{
    Q_OBJECT

public:
    explicit ModifyTransitionCommand(Transition *transition, StateModel *model, QUndoCommand *parent = nullptr);

    int id() const override
    {
        return ModifyTransition;
    }

    Q_INVOKABLE void setSourceState(KDSME::State *sourceState);
    Q_INVOKABLE void setTargetState(KDSME::State *targetState);
    Q_INVOKABLE void setShape(const QPainterPath &shape);

    void redo() override;
    void undo() override;
    bool mergeWith(const QUndoCommand *other) override;

private:
    void updateText();

    QPointer<Transition> m_transition;

    enum Operation
    {
        NoOperation,
        SetSourceStateOperation,
        SetTargetStateOperation,
        SetShapeOperation
    } m_operation;

    QPointer<State> m_sourceState, m_oldSourceState;
    QPointer<State> m_targetState, m_oldTargetState;
    QPainterPath m_shape, m_oldShape;
};

}

#endif // MODIFYTRANSITIONCOMMAND_P_H
