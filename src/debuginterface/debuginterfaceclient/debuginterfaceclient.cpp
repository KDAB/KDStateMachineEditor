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

#include "debuginterfaceclient.h"

#include <config-kdsme.h>

#include "rep_debuginterface_replica.h"

#include "runtimecontroller.h"
#include "state.h"
#include "transition.h"

#include <QDebug>

#define IF_DEBUG(x)

using namespace KDSME;
using namespace KDSME::DebugInterface;

namespace {

RuntimeController::Configuration toSmeConfiguration(const StateMachineConfiguration &config,
                                                    const QHash<StateId, State *> &map)
{
    RuntimeController::Configuration result;
    for (const StateId &id : config) {
        if (auto state = map.value(id)) {
            result << state;
        }
    }
    return result;
}

}

struct DebugInterfaceClient::Private : public QObject
{
    Q_OBJECT

public:
    Private(DebugInterfaceClient *q)
        : q(q)
        , m_debugInterface(nullptr)
        , m_machine(nullptr)
    {
        DebugInterface::registerTypes();
    }

public Q_SLOTS:
    void showMessage(const QString &message);
    void stateAdded(const DebugInterface::StateId stateId, const DebugInterface::StateId parentId, const bool hasChildren,
                    const QString &label, const DebugInterface::StateType type, const bool connectToInitial);
    void stateConfigurationChanged(const DebugInterface::StateMachineConfiguration &config);
    void transitionAdded(const DebugInterface::TransitionId transitionId, const DebugInterface::StateId source,
                         const DebugInterface::StateId target, const QString &label);
    void statusChanged(const bool haveStateMachine, const bool running);
    void transitionTriggered(DebugInterface::TransitionId transition, const QString &label);

    void repopulateView();
    void clearGraph();

    void stateChanged(QRemoteObjectReplica::State state);

public:
    DebugInterfaceClient *q;
    DebugInterfaceReplica *m_debugInterface;

    QHash<DebugInterface::StateId, State *> m_idToStateMap;
    QHash<DebugInterface::TransitionId, Transition *> m_idToTransitionMap;
    StateMachine *m_machine;
};

DebugInterfaceClient::DebugInterfaceClient(QObject *parent)
    : RuntimeController(parent)
    , d(new Private(this))
{
}

DebugInterfaceClient::~DebugInterfaceClient()
{
}

DebugInterfaceReplica *DebugInterfaceClient::debugInterface() const
{
    return d->m_debugInterface;
}

void DebugInterfaceClient::setDebugInterface(DebugInterfaceReplica *debugInterface)
{
    if (d->m_debugInterface == debugInterface)
        return;

    if (d->m_debugInterface) {
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::message,
                   d.data(), &Private::showMessage);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::stateConfigurationChanged,
                   d.data(), &Private::stateConfigurationChanged);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::stateAdded,
                   d.data(), &Private::stateAdded);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::transitionAdded,
                   d.data(), &Private::transitionAdded);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::statusChanged,
                   d.data(), &Private::statusChanged);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::transitionTriggered,
                   d.data(), &Private::transitionTriggered);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::aboutToRepopulateGraph,
                   d.data(), &Private::clearGraph);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::graphRepopulated,
                   d.data(), &Private::repopulateView);
        disconnect(d->m_debugInterface, &DebugInterfaceReplica::stateChanged,
                   d.data(), &Private::stateChanged);

        d->clearGraph();
    }

    d->m_debugInterface = debugInterface;

    if (d->m_debugInterface) {
        connect(d->m_debugInterface, &DebugInterfaceReplica::message,
                d.data(), &Private::showMessage);
        connect(d->m_debugInterface, &DebugInterfaceReplica::stateConfigurationChanged,
                d.data(), &Private::stateConfigurationChanged);
        connect(d->m_debugInterface, &DebugInterfaceReplica::stateAdded,
                d.data(), &Private::stateAdded);
        connect(d->m_debugInterface, &DebugInterfaceReplica::transitionAdded,
                d.data(), &Private::transitionAdded);
        connect(d->m_debugInterface, &DebugInterfaceReplica::statusChanged,
                d.data(), &Private::statusChanged);
        connect(d->m_debugInterface, &DebugInterfaceReplica::transitionTriggered,
                d.data(), &Private::transitionTriggered);
        connect(d->m_debugInterface, &DebugInterfaceReplica::aboutToRepopulateGraph,
                d.data(), &Private::clearGraph);
        connect(d->m_debugInterface, &DebugInterfaceReplica::graphRepopulated,
                d.data(), &Private::repopulateView);
        connect(d->m_debugInterface, &DebugInterfaceReplica::stateChanged,
                d.data(), &Private::stateChanged);

        d->m_debugInterface->repopulateGraph();
    }
}

StateMachine *DebugInterfaceClient::machine() const
{
    return d->m_machine;
}

void DebugInterfaceClient::Private::showMessage(const QString &message)
{
    Q_UNUSED(message);
    // FIXME: Port
}

void DebugInterfaceClient::Private::stateConfigurationChanged(const StateMachineConfiguration &config)
{
    const auto smeConfig = toSmeConfiguration(config, m_idToStateMap);
    q->setActiveConfiguration(smeConfig);
}

void DebugInterfaceClient::Private::stateAdded(const StateId stateId, const StateId parentId, const bool hasChildren,
                                               const QString &label, const StateType type, const bool connectToInitial)
{
    Q_UNUSED(hasChildren);
    IF_DEBUG(qDebug() << "stateAdded" << stateId << parentId << hasChildren << label << type << connectToInitial);

    if (m_idToStateMap.contains(stateId)) {
        return;
    }

    State *parentState = m_idToStateMap.value(parentId);
    State *state = nullptr;
    if (type == StateMachineState) {
        state = m_machine = new StateMachine;
        m_machine->setRuntimeController(q);
    } else if (type == DebugInterface::FinalState) {
        state = new FinalState(parentState);
    } else if (type == DebugInterface::ShallowHistoryState) {
        state = new HistoryState(HistoryState::ShallowHistory, parentState);
    } else if (type == DebugInterface::DeepHistoryState) {
        state = new HistoryState(HistoryState::DeepHistory, parentState);
    } else {
        state = new State(parentState);
    }

    if (connectToInitial && parentState) {
        State *initialState = new PseudoState(PseudoState::InitialState, parentState);
        initialState->setFlags(Element::ElementIsSelectable);
        Transition *transition = new Transition(initialState);
        transition->setTargetState(state);
        transition->setFlags(Element::ElementIsSelectable);
    }

    Q_ASSERT(state);
    state->setLabel(label);
    state->setInternalId(stateId);
    state->setFlags(Element::ElementIsSelectable);
    m_idToStateMap[stateId] = state;
}

void DebugInterfaceClient::Private::transitionAdded(const TransitionId transitionId, const StateId sourceId, const StateId targetId, const QString &label)
{
    if (m_idToTransitionMap.contains(transitionId))
        return;

    IF_DEBUG(qDebug() << transitionId << label << sourceId << targetId);

    State *source = m_idToStateMap.value(sourceId);
    State *target = m_idToStateMap.value(targetId);
    if (!source || !target) {
        qDebug() << "Null source or target for transition:" << transitionId;
        return;
    }

    Transition *transition = new Transition(source);
    transition->setTargetState(target);
    transition->setLabel(label);
    transition->setFlags(Element::ElementIsSelectable);
    m_idToTransitionMap[transitionId] = transition;
}

void DebugInterfaceClient::Private::statusChanged(const bool haveStateMachine, const bool running)
{
    Q_UNUSED(haveStateMachine);
    IF_DEBUG(qDebug() << haveStateMachine << running);

    if (m_machine) {
        q->setIsRunning(running);
    }
}

void DebugInterfaceClient::Private::transitionTriggered(TransitionId transitionId, const QString &label)
{
    Q_UNUSED(label);
    q->setLastTransition(m_idToTransitionMap.value(transitionId));
}

void DebugInterfaceClient::Private::clearGraph()
{
    IF_DEBUG(qDebug());

    m_idToStateMap.clear();
    m_idToTransitionMap.clear();

    Q_EMIT q->clearGraph();
}

void DebugInterfaceClient::Private::stateChanged(QRemoteObjectReplica::State state)
{
    if (state == QRemoteObjectReplica::Valid) {
        m_debugInterface->repopulateGraph();
    } else {
        clearGraph();
    }
}

void DebugInterfaceClient::Private::repopulateView()
{
    IF_DEBUG(qDebug() << m_machine);

    Q_EMIT q->repopulateView();
}

#include "debuginterfaceclient.moc"
