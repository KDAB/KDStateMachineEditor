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

#ifndef KDSME_COMMAND_CREATEELEMENTCOMMAND_P_H
#define KDSME_COMMAND_CREATEELEMENTCOMMAND_P_H

#include "command_p.h"
#include "element.h"

namespace KDSME {

class KDSME_VIEW_EXPORT CreateElementCommand : public Command
{
    Q_OBJECT
    Q_PROPERTY(KDSME::Element *parentElement READ parentElement WRITE setParentElement NOTIFY parentElementChanged)
    Q_PROPERTY(KDSME::Element::Type type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit CreateElementCommand(StateModel *model = nullptr,
                                  Element::Type type = Element::ElementType,
                                  QUndoCommand *parent = nullptr);
    ~CreateElementCommand();

    Element *parentElement() const;
    void setParentElement(Element *parentElement);

    Element::Type type() const;
    void setType(Element::Type type);

    Element *createdElement() const;

    int id() const override
    {
        return CreateElement;
    }

    void redo() override;
    void undo() override;

Q_SIGNALS:
    void parentElementChanged(KDSME::Element *parentElement);
    void typeChanged(KDSME::Element::Type type);

private:
    void updateText();

    Element *m_parentElement;
    Element::Type m_type;

    Element *m_createdElement;
};

}

#endif
