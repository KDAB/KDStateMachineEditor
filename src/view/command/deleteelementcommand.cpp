/*
  DeleteElementCommand.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "deleteelementcommand.h"

#include "elementfactory.h"
#include "layoutimportexport.h"
#include "elementmodel.h"
#include "statemachinescene.h"

#include "debug.h"

using namespace KDSME;

DeleteElementCommand::DeleteElementCommand(StateMachineScene* scene, Element* deletedElement, QUndoCommand* parent)
    : Command(scene->stateModel(), parent)
    , m_scene(scene)
    , m_index(-1)
    , m_parentElement(nullptr)
    , m_deletedElement(deletedElement)
{
    updateText();
}

DeleteElementCommand::~DeleteElementCommand()
{
    if (m_parentElement) {
        delete m_deletedElement;
    }
}

void DeleteElementCommand::redo()
{
    qCDebug(KDSME_VIEW) << Q_FUNC_INFO;

    Q_ASSERT(m_deletedElement);

    m_parentElement = m_deletedElement->parentElement();

    QModelIndex index = model()->indexForObject(m_deletedElement);
    Q_ASSERT(index.isValid());
    m_index = index.row();

    StateModel::RemoveOperation remove(model(), m_deletedElement);
    Q_UNUSED(remove);
    m_deletedElement->setParent(nullptr);
}

void DeleteElementCommand::undo()
{
    qCDebug(KDSME_VIEW) << Q_FUNC_INFO;

    if (!m_parentElement) {
        qCDebug(KDSME_VIEW) << Q_FUNC_INFO << "Aborting undo, element was never deleted";
        return;
    }

    {
        const int count = 1;
        StateModel::AppendOperation append(model(), m_parentElement, count, m_index);
        Q_UNUSED(append);

        m_deletedElement->setParent(m_parentElement);
    }

    m_parentElement = nullptr;
}

void DeleteElementCommand::updateText()
{
    setText(tr("Delete %1").arg(m_deletedElement ? m_deletedElement->toDisplayString() : ("<No element>")));
}
