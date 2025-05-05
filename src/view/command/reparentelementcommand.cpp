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

#include "reparentelementcommand_p.h"

#include "element.h"
#include "elementmodel.h"
#include "statemachinescene.h"

#include "debug.h"

using namespace KDSME;

ReparentElementCommand::ReparentElementCommand(StateMachineScene *view, Element *element, QUndoCommand *parent)
    : Command(tr("Set parent of '%1'").arg(element->label()), parent)
    , m_view(view)
    , m_element(element)
    , m_valid(true)
{
    Q_ASSERT(view);
    Q_ASSERT(element);
}

void ReparentElementCommand::setParentElement(Element *parentElement)
{
    m_newParentElement = parentElement; // NOLINT(clang-analyzer-cplusplus.NewDelete)
}

void ReparentElementCommand::redo()
{
    if (!m_element)
        return;

    m_valid = m_element->parentElement();
    if (!m_valid) {
        qCDebug(KDSME_VIEW) << "No Element* as parent, not reparenting this item";
        return;
    }

    m_oldParentElement = m_element->parentElement();

    const ObjectTreeModel::ReparentOperation reparentOperation(m_view->stateModel(), m_element, m_newParentElement);
    m_element->setParent(m_newParentElement);
}

void ReparentElementCommand::undo()
{
    if (!m_element)
        return;

    if (!m_valid)
        return;

    const ObjectTreeModel::ReparentOperation reparentOperation(m_view->stateModel(), m_element, m_oldParentElement);
    m_element->setParent(m_oldParentElement);
}
