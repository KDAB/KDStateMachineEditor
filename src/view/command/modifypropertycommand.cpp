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

#include "modifypropertycommand_p.h"

#include "debug.h"
#include <QJsonObject>
#include <QVariant>

using namespace KDSME;

ModifyPropertyCommand::ModifyPropertyCommand(QObject *object, const char *property,
                                             const QVariant &value, const QString &text, QUndoCommand *parent)
    : Command(text, parent)
    , m_object(object)
{
    m_propertyMap.insert(property, value);
    init();
}

ModifyPropertyCommand::ModifyPropertyCommand(QObject *object, const QJsonObject &propertyMap,
                                             const QString &text, QUndoCommand *parent)
    : Command(text, parent)
    , m_object(object)
{
    auto it = propertyMap.constBegin();
    while (it != propertyMap.constEnd()) {
        m_propertyMap.insert(it.key().toLatin1(), it.value().toVariant());
        ++it;
    }
    init();
}

void ModifyPropertyCommand::init()
{
    if (text().isEmpty() && !m_propertyMap.isEmpty()) {
        if (m_propertyMap.size() == 1) {
            setText(tr("Modify property '%1'").arg(m_propertyMap.begin().value().toString()));
        } else {
            setText(tr("Modify multiple properties"));
        }
    } else {
        setText(tr("N/A"));
    }
}

void ModifyPropertyCommand::redo()
{
    if (!m_object) {
        qCDebug(KDSME_VIEW) << "Invalid object";
        return;
    }

    auto it = m_propertyMap.constBegin();
    while (it != m_propertyMap.constEnd()) {
        m_oldPropertyMap.insert(it.key(), m_object->property(it.key().constData()));
        if (!m_object->setProperty(it.key().constData(), it.value())) {
            qCDebug(KDSME_VIEW) << "Failed to set property" << it.key();
        }
        ++it;
    }
}

void ModifyPropertyCommand::undo()
{
    if (!m_object) {
        qCDebug(KDSME_VIEW) << "Invalid object";
        return;
    }

    auto it = m_oldPropertyMap.constBegin();
    while (it != m_oldPropertyMap.constEnd()) {
        if (!m_object->setProperty(it.key().constData(), it.value())) {
            qCDebug(KDSME_VIEW) << "Failed to set property" << it.key();
        }
        ++it;
    }
    m_oldPropertyMap.clear();
}
