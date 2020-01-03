/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_COMMAND_CREATEELEMENTCOMMAND_P_H
#define KDSME_COMMAND_CREATEELEMENTCOMMAND_P_H

#include "command_p.h"
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
    ~CreateElementCommand();

    Element* parentElement() const;
    void setParentElement(Element* parentElement);

    Element::Type type() const;
    void setType(Element::Type type);

    Element* createdElement() const;

    int id() const override { return CreateElement; }

    void redo() override;
    void undo() override;

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
