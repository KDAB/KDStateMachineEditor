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

#ifndef KDSME_COMMAND_COMMAND_P_H
#define KDSME_COMMAND_COMMAND_P_H

#include "kdsme_view_export.h"

#include <QUndoCommand>

namespace KDSME {
class StateModel;

class KDSME_VIEW_EXPORT Command : public QObject, public QUndoCommand // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public:
    enum Id
    {
        CreateElement = 0,
        DeleteElement,
        ReparentElement,
        LayoutSnapshot,
        ModifyProperty,
        ModifyInitialState,
        ModifyDefaultState,
        ModifyElement,
        ModifyTransition,
        ChangeStateMachine
    };

    explicit Command(StateModel *model, QUndoCommand *parent = nullptr);
    explicit Command(const QString &text = QString(), QUndoCommand *parent = nullptr);

    StateModel *model() const;

private:
    StateModel *m_model;
};

}

Q_DECLARE_METATYPE(KDSME::Command *)

#endif // COMMAND_P_H
