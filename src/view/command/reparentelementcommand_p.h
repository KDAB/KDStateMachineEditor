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

#ifndef KDSME_COMMAND_REPARENTELEMENTCOMMAND_P_H
#define KDSME_COMMAND_REPARENTELEMENTCOMMAND_P_H

#include "command_p.h"

#include <QPointer>

namespace KDSME {

class Element;
class StateMachineScene;

/**
 * @brief Modifies the parent element of a @ref KDSME::Element
 *
 * This command is useful when dragging & dropping elements
 * into a different level in the state chart hierarchy.
 * On undo, the previous parent is restored.
 */
class KDSME_VIEW_EXPORT ReparentElementCommand : public Command
{
    Q_OBJECT

public:
    ReparentElementCommand(StateMachineScene *view, Element *element, QUndoCommand *parent = nullptr);

    int id() const override
    {
        return ReparentElement;
    }

    Q_INVOKABLE void setParentElement(KDSME::Element *parentElement);

    void redo() override;
    void undo() override;

private:
    QPointer<StateMachineScene> m_view;
    QPointer<Element> m_element;

    /// Whether this command is valid (ie. if redo/undo is doing something)
    bool m_valid;

    QPointer<Element> m_newParentElement;
    QPointer<Element> m_oldParentElement;
};

}

#endif // REPARENTELEMENTCOMMAND_P_H
