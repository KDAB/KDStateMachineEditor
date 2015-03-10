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
1
  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Contact info@kdab.com if any conditions of this licensing are not
  clear to you.
*/

#include "qsmadapter.h"

#include "qsmdebuginterfacesource.h"
#include "debuginterface_replica.h"

#include <QRemoteObjectNode>

using namespace KDSME;

struct QsmAdapter::Private
{
    QRemoteObjectNode m_registryHostNode;
    QRemoteObjectNode m_hostNode;
    QRemoteObjectNode m_client;
    QsmDebugInterfaceSource m_interface;
};

QsmAdapter::QsmAdapter(QObject* parent)
    : DebugInterfaceClient(parent)
    , d(new Private)
{
    // set up the debug interface on the local registry and connect to it
    // this is simpler than writing another class that handles in-process debuggging
    // just pay the cost for the in-process communication, it's not that much anyway
    d->m_registryHostNode = QRemoteObjectNode::createRegistryHostNode();
    d->m_hostNode = QRemoteObjectNode::createHostNodeConnectedToRegistry();
    d->m_hostNode.enableRemoting(d->m_interface.remoteObjectSource());

    d->m_client = QRemoteObjectNode::createNodeConnectedToRegistry();
    auto interfaceReplica = d->m_client.acquire<DebugInterfaceReplica>();
    interfaceReplica->waitForSource();
    setDebugInterface(interfaceReplica);
}

QsmAdapter::~QsmAdapter()
{
}

QStateMachine* QsmAdapter::qStateMachine() const
{
    return d->m_interface.qStateMachine();
}

void QsmAdapter::setQStateMachine(QStateMachine* machine)
{
    d->m_interface.setQStateMachine(machine);
}
