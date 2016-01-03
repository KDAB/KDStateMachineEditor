/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_DEBUGINTERFACECLIENT_H
#define KDSME_DEBUGINTERFACECLIENT_H

#include "kdsme_debuginterfaceclient_export.h"

#include "runtimecontroller.h"

class DebugInterfaceReplica;

namespace KDSME {
class State;
class StateMachine;
class Transition;

class KDSME_DEBUGINTERFACECLIENT_EXPORT DebugInterfaceClient : public RuntimeController
{
    Q_OBJECT

public:
    explicit DebugInterfaceClient(QObject* parent = nullptr);
    ~DebugInterfaceClient();

public:
    DebugInterfaceReplica* debugInterface() const;
    void setDebugInterface(DebugInterfaceReplica* debugInterface);

    KDSME::StateMachine* machine() const;

Q_SIGNALS:
    void repopulateView();
    void clearGraph();

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // KDSME_DEBUGINTERFACECLIENT_H
