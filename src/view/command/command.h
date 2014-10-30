/*
  command.h

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

#ifndef KDSME_COMMAND_COMMAND_H
#define KDSME_COMMAND_COMMAND_H

#include "kdsme_view_export.h"

#include <QUndoCommand>

namespace KDSME {
class StateModel;

class KDSME_VIEW_EXPORT Command : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    enum Id {
        CreateElement = 0,
        DeleteElement,
        LayoutSnapshot,
        ModifyProperty,
        ModifyInitialState,
        ModifyDefaultState,

        ReparentElement,

        ModifyTransition,

        ModifyLayoutItem,
        ModifyTransitionLayoutItem,

        ChangeStateMachine
    };

    explicit Command(StateModel* model, QUndoCommand* parent = nullptr);
    explicit Command(const QString& text = QString(), QUndoCommand* parent = nullptr);

    StateModel* model() const;

private:
    StateModel* m_model;
};

}

Q_DECLARE_METATYPE(KDSME::Command*)

#endif // COMMAND_H
