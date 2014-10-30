/*
  createelementcommand.h

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

#ifndef KDSME_COMMAND_CREATEELEMENTCOMMAND_H
#define KDSME_COMMAND_CREATEELEMENTCOMMAND_H

#include "command.h"
#include "element.h"

namespace KDSME {

class KDSME_VIEW_EXPORT CreateElementCommand : public Command
{
    Q_OBJECT
    Q_PROPERTY(KDSME::Element* parentElement READ parentElement WRITE setParentElement NOTIFY parentElementChanged)
    Q_PROPERTY(KDSME::Element::Type type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit CreateElementCommand(StateModel* model = nullptr,
				  Element::Type type = Element::ElementType,
				  QUndoCommand* parent = nullptr);

    Element* parentElement() const;
    void setParentElement(Element* parentElement);

    Element::Type type() const;
    void setType(Element::Type type);

    Element* createdElement() const;

    virtual int id() const Q_DECL_OVERRIDE { return CreateElement; }

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void parentElementChanged(Element* parentElement);
    void typeChanged(Element::Type type);

private:
    void updateText();

    Element* m_parentElement;
    Element::Type m_type;

    Element* m_createdElement;
};

}

#endif
