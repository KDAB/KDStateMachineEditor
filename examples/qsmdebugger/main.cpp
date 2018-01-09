/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "config-examples.h"

#if Qt5RemoteObjects_VERSION < QT_VERSION_CHECK(5, 9, 0)
#include "debuginterface_replica.h"
#else
#include "rep_debuginterface_replica.h"
#endif

#include "trafficlight.h"

#include <debuginterfaceclient.h>
#include <qsmdebuginterfacesource.h>
#include <state.h>
#include <statemachinescene.h>
#include <statemachineview.h>

#include <QApplication>
#include <QRemoteObjectNode>

using namespace KDSME;

/**
 * @brief Create a GUI driven by a state machine, realtime-debug this state machine in another window
 */
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    //! [Target setup]
    TrafficLight trafficLight;
    trafficLight.resize(110, 300);
    trafficLight.show();

    // set up the debug interface on the local registry and connect to it
    // this is simpler than writing another class that handles in-process debuggging
    // just pay the cost for the in-process communication, it's not that much anyway
    QRemoteObjectRegistryHost registryHostNode(QUrl(QStringLiteral("local:registry")));
    QRemoteObjectHost hostNode(QUrl(QStringLiteral("local:replica")), QUrl(QStringLiteral("local:registry")));
    QsmDebugInterfaceSource interfaceSource;
    interfaceSource.setQStateMachine(trafficLight.machine());
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
        }
    );
    //! [Client setup for viewing the state machine]

    app.exec();
}
