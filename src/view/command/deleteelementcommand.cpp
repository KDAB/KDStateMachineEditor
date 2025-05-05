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

#include "deleteelementcommand_p.h"

#include "elementfactory.h"
#include "layoutimportexport.h"
#include "elementmodel.h"
#include "statemachinescene.h"

#include "debug.h"

using namespace KDSME;

DeleteElementCommand::DeleteElementCommand(StateMachineScene *scene, Element *deletedElement, QUndoCommand *parent)
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
    delete m_deletedElement;
} // NOLINT(clang-analyzer-cplusplus.NewDelete)

void DeleteElementCommand::redo()
{
    qCDebug(KDSME_VIEW) << Q_FUNC_INFO;

    Q_ASSERT(m_deletedElement);

    m_parentElement = m_deletedElement->parentElement();

    const QModelIndex index = model()->indexForObject(m_deletedElement);
    Q_ASSERT(index.isValid());
    m_index = index.row();

    const StateModel::RemoveOperation remove(model(), m_deletedElement);
    Q_UNUSED(remove);
    m_deletedElement->setParent(nullptr);
}

void DeleteElementCommand::undo()
{
    qCDebug(KDSME_VIEW) << Q_FUNC_INFO;

    if (!m_parentElement) {
        qCDebug(KDSME_VIEW) << "Aborting undo, element was never deleted";
        return;
    }

    {
        const int count = 1;
        const StateModel::AppendOperation append(model(), m_parentElement, count, m_index);
        Q_UNUSED(append);

        m_deletedElement->setParent(m_parentElement);
    }

    m_parentElement = nullptr;
}

void DeleteElementCommand::updateText()
{
    setText(tr("Delete %1").arg(m_deletedElement ? m_deletedElement->toDisplayString() : QStringLiteral("<No element>")));
}
