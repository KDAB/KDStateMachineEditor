/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
    explicit DeleteElementCommand(StateMachineScene* scene, Element* deletedElement, QUndoCommand* parent = nullptr);
    ~DeleteElementCommand();

    int id() const override { return DeleteElement; }

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
