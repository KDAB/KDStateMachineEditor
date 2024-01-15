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
    explicit DebugInterfaceClient(QObject *parent = nullptr);
    ~DebugInterfaceClient();

public:
    DebugInterfaceReplica *debugInterface() const;
    void setDebugInterface(DebugInterfaceReplica *debugInterface);

    KDSME::StateMachine *machine() const;

Q_SIGNALS:
    void repopulateView();
    void clearGraph();

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // KDSME_DEBUGINTERFACECLIENT_H
