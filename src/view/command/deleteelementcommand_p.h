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

#ifndef KDSME_COMMAND_DELETEELEMENTCOMMAND_P_H
#define KDSME_COMMAND_DELETEELEMENTCOMMAND_P_H

#include "command_p.h"
#include "element.h"

#include <QPointer>
#include <QJsonObject>

namespace KDSME {

class StateMachineScene;

/**
 * @brief This is the inverse operation to the @ref KDSME::CreateElementCommand command
 *
 * On redo it removes an element from the model, but still keeping a reference on it.
 * On undo this element is registered in the model again.
 */
class KDSME_VIEW_EXPORT DeleteElementCommand : public Command
{
    Q_OBJECT

public:
    explicit DeleteElementCommand(StateMachineScene *scene, Element *deletedElement, QUndoCommand *parent = nullptr);
    ~DeleteElementCommand();

    int id() const override
    {
        return DeleteElement;
    }

    void redo() override;
    void undo() override;

private:
    void updateText();

    QPointer<StateMachineScene> m_scene;
    int m_index;
    QJsonObject m_layout;
    QPointer<Element> m_parentElement;
    QPointer<Element> m_deletedElement;
};

}

#endif
