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

#include "createelementcommand_p.h"

#include "elementfactory.h"
#include "elementmodel.h"
#include "state.h"

#include "debug.h"

using namespace KDSME;

CreateElementCommand::CreateElementCommand(StateModel *model, const Element::Type type, QUndoCommand *parent)
    : Command(model, parent)
    , m_parentElement(nullptr)
    , m_type(type)
    , m_createdElement(nullptr)
{
    updateText();
}

CreateElementCommand::~CreateElementCommand()
{
    if (m_createdElement && !m_createdElement->parent())
        delete m_createdElement;
}

Element *CreateElementCommand::parentElement() const
{
    return m_parentElement;
}

void CreateElementCommand::setParentElement(Element *parentElement)
{
    if (m_parentElement == parentElement)
        return;

    m_parentElement = parentElement;
    Q_EMIT parentElementChanged(m_parentElement);
}

Element::Type CreateElementCommand::type() const
{
    return m_type;
}

void CreateElementCommand::setType(Element::Type type)
{
    if (m_type == type)
        return;

    m_type = type;
    Q_EMIT typeChanged(m_type);
}

Element *CreateElementCommand::createdElement() const
{
    return m_createdElement;
}

void CreateElementCommand::redo()
{
    static const ElementFactory factory;

    if (!model() || m_type == Element::ElementType) {
        qCDebug(KDSME_VIEW) << "Can't redo, invalid model or type";
        return;
    }

    Element *parentElementL = m_parentElement ? m_parentElement : model()->state();
    const StateModel::AppendOperation append(model(), parentElementL);
    if (m_createdElement) {
        m_createdElement->setParent(parentElementL);
    } else {
        Element *element = factory.create(m_type);
        if (!element) {
            qCDebug(KDSME_VIEW) << "Element could not be instantiated, type:" << m_type;
            return;
        }

        element->setLabel(tr("Unnamed"));
        element->setParent(parentElementL);
        m_createdElement = element;
    }
    updateText();
}

void CreateElementCommand::undo()
{
    if (!m_createdElement) {
        qCDebug(KDSME_VIEW) << "Aborting undo, element was never created";
        return;
    }

    {
        const StateModel::RemoveOperation remove(model(), m_createdElement);
        m_createdElement->setParent(nullptr);
    }
}

void CreateElementCommand::updateText()
{
    setText(tr("Create %1").arg(m_createdElement ? m_createdElement->toDisplayString() : QStringLiteral("<No element>")));
}
