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

#include "modifyelementcommand_p.h"

#include "element.h"

#include "debug.h"

using namespace KDSME;

namespace {

void setElementGeometry(Element *item, const QRectF &geometry)
{
    item->setPos(geometry.topLeft());
    item->setWidth(geometry.width());
    item->setHeight(geometry.height());
}

}

KDSME::ModifyElementCommand::ModifyElementCommand(Element *item, QUndoCommand *parent)
    : Command(QString(), parent)
    , m_operation(NoOperation)
    , m_item(item)
{
    Q_ASSERT(item);
}

Element *KDSME::ModifyElementCommand::item() const
{
    return m_item;
}

void KDSME::ModifyElementCommand::moveBy(qreal dx, qreal dy)
{
    m_moveByData = QPointF(dx, dy);
    m_operation = MoveOperation;
    updateText();
}

void ModifyElementCommand::setGeometry(const QRectF &geometry)
{
    m_newGeometry = geometry;
    m_operation = SetGeometryOperation;
    updateText();
}

void KDSME::ModifyElementCommand::redo()
{
    if (!m_item || m_operation == NoOperation) {
        qCDebug(KDSME_VIEW) << "Invalid item or no operation requested";
        return;
    }

    switch (m_operation) {
    case MoveOperation:
        m_item->setPos(m_item->pos() + m_moveByData);
        break;
    case SetGeometryOperation:
        m_oldGeometry = QRectF(m_item->pos(), QSizeF(m_item->width(), m_item->height()));
        setElementGeometry(m_item.data(), m_newGeometry);
        break;
    }
}

void KDSME::ModifyElementCommand::undo()
{
    if (!m_item || m_operation == NoOperation) {
        qCDebug(KDSME_VIEW) << "Invalid item or no operation requested";
        return;
    }

    switch (m_operation) {
    case MoveOperation:
        m_item->setPos(m_item->pos() - m_moveByData);
        break;
    case SetGeometryOperation:
        setElementGeometry(m_item.data(), m_oldGeometry);
        break;
    }
}

bool ModifyElementCommand::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) {
        return false;
    }

    auto cmd = static_cast<const ModifyElementCommand *>(other);
    if (cmd->m_item != m_item || cmd->m_operation != m_operation) {
        return false;
    }

    m_moveByData += cmd->m_moveByData;
    m_newGeometry = cmd->m_newGeometry;
    return true;
}

void KDSME::ModifyElementCommand::updateText()
{
    const QString itemLabel = m_item ? m_item->label() : tr("<Unknown>");

    switch (m_operation) {
    case MoveOperation:
        setText(tr("Moving item %1").arg(itemLabel));
        break;
    case SetGeometryOperation:
        setText(tr("Resizing item %1").arg(itemLabel));
        break;
    default:
        setText(QString());
        break;
    }
}
