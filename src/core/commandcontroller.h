/*
  commandcontroller.h

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

#ifndef KDSME_COMMANDCONTROLLER_H
#define KDSME_COMMANDCONTROLLER_H

#include "kdsme_core_export.h"

#include <QObject>

class QUndoCommand;
class QUndoStack;

namespace KDSME {
class Command;
class State;
class StateModel;

class KDSME_CORE_EXPORT CommandController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUndoStack* undoStack READ undoStack CONSTANT)

public:
    explicit CommandController(QUndoStack* undoStack, QObject* parent = 0);
    virtual ~CommandController();

    Q_INVOKABLE void push(KDSME::Command* command);

    void clear();

    QUndoStack* undoStack() const;

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::CommandController*)

#endif // COMMANDCONTROLLER_H
