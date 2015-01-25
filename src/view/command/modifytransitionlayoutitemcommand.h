/*
  modifytransitionlayoutitemcommand.h

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

#ifndef KDSME_COMMAND_MODIFYTRANSITIONLAYOUTITEMCOMMAND_H
#define KDSME_COMMAND_MODIFYTRANSITIONLAYOUTITEMCOMMAND_H

#include "command.h"

#include <QPainterPath>
#include <QPointer>

namespace KDSME {

class Transition;

class KDSME_VIEW_EXPORT ModifyTransitionLayoutItemCommand : public Command
{
    Q_OBJECT

public:
    explicit ModifyTransitionLayoutItemCommand(Transition* item, QUndoCommand* parent = nullptr);

    virtual int id() const override { return ModifyTransitionLayoutItem; }

    virtual void redo() override;
    virtual void undo() override;
    virtual bool mergeWith(const QUndoCommand* other) override;

    Q_INVOKABLE void setShape(const QPainterPath& path);

private:
    void updateText();

    QPointer<Transition> m_item;

    enum Operation {
        NoOperation,
        SetShapeOperation
    } m_operation;

    QPainterPath m_shape, m_oldShape;
};

}

#endif // MODIFYTRANSITIONLAYOUTITEM_H
