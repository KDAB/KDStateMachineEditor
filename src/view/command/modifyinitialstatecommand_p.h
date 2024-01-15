/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_COMMAND_MODIFYINITIALSTATECOMMAND_P_H
#define KDSME_COMMAND_MODIFYINITIALSTATECOMMAND_P_H

#include "command_p.h"

#include <QPointer>

namespace KDSME {

class State;

/**
 * @brief Modifies the initial state of a @ref KDSME::State
 *
 * On undo the previous initial state is restored.
 */
class KDSME_VIEW_EXPORT ModifyInitialStateCommand : public Command
{
    Q_OBJECT
public:
    explicit ModifyInitialStateCommand(State *state, State *initialState, QUndoCommand *parent = nullptr);
    ~ModifyInitialStateCommand();

    int id() const override;
    void undo() override;
    void redo() override;

private:
    QPointer<State> m_state;
    QPointer<State> m_initialState;
    QPointer<State> m_oldInitialState;
};

}

#endif // KDSME_MODIFYINITIALSTATECOMMAND_P_H
