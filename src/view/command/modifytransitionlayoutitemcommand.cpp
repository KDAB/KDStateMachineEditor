/*
  modifytransitionlayoutitemcommand.cpp

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

#include "modifytransitionlayoutitemcommand.h"

#include "element.h"
#include "layoutitem.h"

using namespace KDSME;

ModifyTransitionLayoutItemCommand::ModifyTransitionLayoutItemCommand(TransitionLayoutItem* item, QUndoCommand* parent)
    : Command(QString(), parent)
    , m_item(item)
    , m_operation(NoOperation)
{
}

void ModifyTransitionLayoutItemCommand::redo()
{
    if (!m_item)
        return;

    switch (m_operation) {
    case SetShapeOperation:
        m_oldShape = m_item->shape();
        m_item->setShape(m_shape);
        break;
    default:
        break;
    }
}

void ModifyTransitionLayoutItemCommand::undo()
{
    if (!m_item)
        return;

    switch (m_operation) {
    case SetShapeOperation:
        m_item->setShape(m_oldShape);
        break;
    default:
        break;
    }
}

bool ModifyTransitionLayoutItemCommand::mergeWith(const QUndoCommand* other)
{
    if (other->id() != id()) {
        return false;
    }

    auto cmd = static_cast<const ModifyTransitionLayoutItemCommand*>(other);
    if (cmd->m_item != m_item || cmd->m_operation != m_operation) {
        return false;
    }

    m_shape = cmd->m_shape;
    return true;
}

void ModifyTransitionLayoutItemCommand::setShape(const QPainterPath& shape)
{
    m_shape = shape;
    m_operation = SetShapeOperation;
    updateText();
}

void ModifyTransitionLayoutItemCommand::updateText()
{
    const QString itemLabel = m_item->element() ? m_item->element()->label() : tr("<Unknown>");

    if (m_operation == SetShapeOperation) {
        setText(tr("Modify path of '%1'").arg(itemLabel));
    }
}


#include "moc_modifytransitionlayoutitemcommand.cpp"
