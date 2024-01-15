/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "config-examples.h"

#include <qglobal.h>

#include "rep_debuginterface_replica.h"

#include "statemachine.h"
#include "trafficlight.h"

#include <debuginterfaceclient.h>
#include <qscxmldebuginterfacesource.h>
#include <state.h>
#include <statemachinescene.h>
#include <statemachineview.h>

#include <QApplication>
#include <QRemoteObjectNode>

using namespace KDSME;

/**
 * @brief Create a GUI driven by a state machine, realtime-debug this state machine in another window
 */
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    //! [Target setup]
    TrafficLightStateMachine machine;
    TrafficLight widget(&machine);
    widget.show();
    machine.start();

    // set up the debug interface on the local registry and connect to it
    // this is simpler than writing another class that handles in-process debugging
    // just pay the cost for the in-process communication, it's not that much anyway
    QRemoteObjectRegistryHost registryHostNode(QUrl(QStringLiteral("local:registry")));
    QRemoteObjectHost hostNode(QUrl(QStringLiteral("local:replica")), QUrl(QStringLiteral("local:registry")));
    QScxmlDebugInterfaceSource interfaceSource;
    interfaceSource.setQScxmlStateMachine(&machine);
    hostNode.enableRemoting(interfaceSource.remoteObjectSource());
    //! [Target setup]

    //! [Client setup for viewing the state machine]
    StateMachineView view;
    view.resize(800, 600);
    view.show();

    QRemoteObjectNode clientNode(QUrl(QStringLiteral("local:registry")));
    auto interfaceReplica = clientNode.acquire<DebugInterfaceReplica>();
    interfaceReplica->waitForSource();

    DebugInterfaceClient client;
    client.setDebugInterface(interfaceReplica);
    QObject::connect(&client, &DebugInterfaceClient::repopulateView,
                     [&]() {
                         qDebug() << "Updating state machine in view";
                         view.scene()->setRootState(client.machine());
                         view.scene()->layout();
                     });
    //! [Client setup for viewing the state machine]

    app.exec();
}
