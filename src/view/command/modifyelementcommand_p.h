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

#ifndef KDSME_COMMAND_MODIFYELEMENTCOMMAND_P_H
#define KDSME_COMMAND_MODIFYELEMENTCOMMAND_P_H

#include "command_p.h"

#include <QPointer>
#include <QPointF>
#include <QRectF>

namespace KDSME {

class Element;

/**
 * @brief Command for modifying properties of KDSME::Element
 *
 * A command can only set one property at the time.
 * On undo, the previous value of the property is restored.
 */
class KDSME_VIEW_EXPORT ModifyElementCommand : public Command
{
    Q_OBJECT

public:
    explicit ModifyElementCommand(Element *item, QUndoCommand *parent = nullptr);

    Element *item() const;

    int id() const override
    {
        return ModifyElement;
    }

    Q_INVOKABLE void moveBy(qreal dx, qreal dy);
    Q_INVOKABLE void setGeometry(const QRectF &geometry);

    void redo() override;
    void undo() override;
    bool mergeWith(const QUndoCommand *other) override;

protected:
    enum Operation
    {
        NoOperation,
        MoveOperation,
        SetGeometryOperation,

        /// Subclass takes care of executing this operation
        UserOperation = 100
    };
    int m_operation;

private:
    void updateText();

    QPointer<Element> m_item;

    // data
    QPointF m_moveByData;
    QRectF m_newGeometry, m_oldGeometry;
};

}

#endif // MODIFYLAYOUTITEMCOMMAND_P_H
