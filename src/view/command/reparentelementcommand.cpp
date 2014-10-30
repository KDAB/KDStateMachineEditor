/*
  reparentelementcommand.cpp

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

#include "reparentelementcommand.h"

#include "element.h"
#include "elementmodel.h"
#include "view.h"

#include <QDebug>

using namespace KDSME;

ReparentElementCommand::ReparentElementCommand(View* view, Element* element, QUndoCommand* parent)
    : Command(tr("Set parent of '%1'").arg(element->label()), parent)
    , m_view(view)
    , m_element(element)
    , m_valid(true)
{
    Q_ASSERT(view);
    Q_ASSERT(element);
}

void ReparentElementCommand::setParentElement(Element* parentElement)
{
    m_newParentElement = parentElement;
}

void ReparentElementCommand::redo()
{
    if (!m_element)
        return;

    m_valid = (!m_element->parent() || m_element->parentElement());
    if (!m_valid) {
        qDebug() << "No Element* as parent, not reparenting this item";
        return;
    }

    m_oldParentElement = m_element->parentElement();

    ObjectTreeModel::ReparentOperation reparentOperation(m_view->stateModel(), m_element, m_newParentElement);
    m_element->setParent(m_newParentElement);
}

void ReparentElementCommand::undo()
{
    if (!m_element)
        return;

    if (!m_valid)
        return;

    ObjectTreeModel::ReparentOperation reparentOperation(m_view->stateModel(), m_element, m_oldParentElement);
    m_element->setParent(m_oldParentElement);
}

#include "moc_reparentelementcommand.cpp"
