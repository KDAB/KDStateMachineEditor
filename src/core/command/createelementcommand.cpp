/*
  createelementcommand.cpp

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

#include "createelementcommand.h"

#include "elementfactory.h"
#include "elementmodel.h"

#include <QDebug>

using namespace KDSME;

CreateElementCommand::CreateElementCommand(StateModel* model, const Element::Type type, QUndoCommand* parent)
    : Command(model, parent)
    , m_parentElement(nullptr)
    , m_type(type)
    , m_createdElement(nullptr)
{
    updateText();
}

Element* CreateElementCommand::parentElement() const
{
    return m_parentElement;
}

void CreateElementCommand::setParentElement(Element* parentElement)
{
    if (m_parentElement == parentElement)
        return;

    m_parentElement = parentElement;
    emit parentElementChanged(m_parentElement);
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
    emit typeChanged(m_type);
}

Element* CreateElementCommand::createdElement() const
{
    return m_createdElement;
}

void CreateElementCommand::redo()
{
    static const ElementFactory factory;

    if (!model() || m_type == Element::ElementType) {
        qDebug() << Q_FUNC_INFO << "Can't redo, invalid model or type";
        return;
    }

    Element* parentElement = m_parentElement ? m_parentElement : model()->state();
    StateModel::AppendOperation append(model(), parentElement);
    Element* element = factory.create(m_type);
    if (!element) {
        qDebug() << Q_FUNC_INFO << "Element could not be instantiated, type:" << m_type;
        return;
    }

    element->setLabel(tr("Unnamed"));
    element->setParent(parentElement);
    m_createdElement = element;
    updateText();
}

void CreateElementCommand::undo()
{
    if (!m_createdElement) {
        qDebug() << Q_FUNC_INFO << "Aborting undo, element was never created";
        return;
    }

    {
        StateModel::RemoveOperation remove(model(), m_createdElement);
        m_createdElement->setParent(nullptr);
    }
    delete m_createdElement;
    m_createdElement = nullptr;
}

void CreateElementCommand::updateText()
{
    setText(tr("Create %1").arg(m_createdElement ? m_createdElement->toDisplayString() : ("<No element>")));
}
