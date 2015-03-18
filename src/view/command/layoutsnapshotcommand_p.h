/*
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

#ifndef KDSME_COMMAND_LAYOUTSNAPSHOTCOMMAND_P_H
#define KDSME_COMMAND_LAYOUTSNAPSHOTCOMMAND_P_H

#include "command_p.h"

#include <QPointer>

namespace KDSME {

class StateMachineScene;

class KDSME_VIEW_EXPORT LayoutSnapshotCommand : public KDSME::Command
{
    Q_OBJECT

public:
    explicit LayoutSnapshotCommand(StateMachineScene* scene, QUndoCommand* parent = nullptr);
    LayoutSnapshotCommand(StateMachineScene* scene, const QString& text, QUndoCommand* parent = nullptr);

    virtual int id() const override { return LayoutSnapshot; }

    virtual void redo() override;
    virtual void undo() override;

private:
    QPointer<StateMachineScene> m_scene;
};

}

#endif // LAYOUTCHANGECOMMAND_P_H
