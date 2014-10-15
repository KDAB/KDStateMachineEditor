/*
  modifylayoutitemcommand.cpp

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

#include "modifylayoutitemcommand.h"

#include "element.h"
#include "layout/layoutitem.h"

#include <QDebug>

using namespace KDSME;

namespace {

void setLayoutItemGeometry(LayoutItem* item, const QRectF& geometry)
{
    item->setPos(geometry.topLeft());
    item->setWidth(geometry.width());
    item->setHeight(geometry.height());
}

}

KDSME::ModifyLayoutItemCommand::ModifyLayoutItemCommand(KDSME::LayoutItem* item, QUndoCommand* parent)
    : Command(QString(), parent)
    , m_operation(NoOperation)
    , m_item(item)
{
    Q_ASSERT(item);
}

KDSME::LayoutItem* KDSME::ModifyLayoutItemCommand::item() const
{
    return m_item;
}

void KDSME::ModifyLayoutItemCommand::moveBy(qreal dx, qreal dy)
{
    m_moveByData = QPointF(dx, dy);
    m_operation = MoveOperation;
    updateText();
}

void ModifyLayoutItemCommand::setGeometry(const QRectF& geometry)
{
    m_newGeometry = geometry;
    m_operation = SetGeometryOperation;
    updateText();
}

void KDSME::ModifyLayoutItemCommand::redo()
{
    if (!m_item || m_operation == NoOperation) {
        qDebug() << Q_FUNC_INFO << "Invalid item or no operation requested";
        return;
    }

    switch (m_operation) {
    case MoveOperation:
        m_item->setPos(m_item->pos() + m_moveByData);
        break;
    case SetGeometryOperation:
        m_oldGeometry = QRectF(m_item->pos(), QSizeF(m_item->width(), m_item->height()));
        setLayoutItemGeometry(m_item.data(), m_newGeometry);
        break;
    }
}

void KDSME::ModifyLayoutItemCommand::undo()
{
    if (!m_item || m_operation == NoOperation) {
        qDebug() << Q_FUNC_INFO << "Invalid item or no operation requested";
        return;
    }

    switch (m_operation) {
    case MoveOperation:
        m_item->setPos(m_item->pos() - m_moveByData);
        break;
    case SetGeometryOperation:
        setLayoutItemGeometry(m_item.data(), m_oldGeometry);
        break;
    }
}

bool ModifyLayoutItemCommand::mergeWith(const QUndoCommand* other)
{
    if (other->id() != id()) {
        return false;
    }

    auto cmd = static_cast<const ModifyLayoutItemCommand*>(other);
    if (cmd->m_item != m_item || cmd->m_operation != m_operation) {
        return false;
    }

    m_moveByData += cmd->m_moveByData;
    m_newGeometry = cmd->m_newGeometry;
    return true;
}

void KDSME::ModifyLayoutItemCommand::updateText()
{
    const QString itemLabel = m_item && m_item->element() ? m_item->element()->label() : tr("<Unknown>");

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

#include "moc_modifylayoutitemcommand.cpp"
