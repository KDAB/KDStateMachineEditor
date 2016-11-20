/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "qscxmldebuginterfacesource.h"

#include "debuginterface_source.h"

#include "objecthelper.h"

#include <QScxmlStateMachine>
#include <private/qscxmlstatemachineinfo_p.h>

using namespace KDSME;
using namespace DebugInterface;

namespace {

StateId makeStateId(QScxmlStateMachineInfo::StateId stateId)
{
    return StateId{static_cast<quint64>(stateId)};
}
TransitionId makeTransitionId(QScxmlStateMachineInfo::TransitionId transitionId)
{
    return TransitionId{static_cast<quint64>(transitionId)};
}
StateType makeStateType(QScxmlStateMachineInfo::StateType stateType)
{
    switch (stateType) {
        case QScxmlStateMachineInfo::InvalidState:
            return StateMachineState;
        case QScxmlStateMachineInfo::NormalState:
            return OtherState;
        case QScxmlStateMachineInfo::ParallelState:
            return OtherState; // FIXME: No ParallelState. Bug.
        case QScxmlStateMachineInfo::FinalState:
            return FinalState;
        case QScxmlStateMachineInfo::ShallowHistoryState:
            return ShallowHistoryState;
        case QScxmlStateMachineInfo::DeepHistoryState:
            return DeepHistoryState;
    }
    Q_UNREACHABLE();
    return OtherState;
}

}

class QScxmlDebugInterfaceSource::Private : public DebugInterfaceSource
{
    Q_OBJECT

public:
    explicit Private(QObject *parent = nullptr);

    QScxmlStateMachine *qScxmlStateMachine() const;
    void setQScxmlStateMachine(QScxmlStateMachine* machine);

private Q_SLOTS:
    void stateEntered(QScxmlStateMachineInfo::StateId state);
    void stateExited(QScxmlStateMachineInfo::StateId state);
    void handleStateConfigurationChanged();
    void handleTransitionTriggered(QScxmlStateMachineInfo::TransitionId);

    void updateStartStop();
    void toggleRunning();

    void repopulateGraph() override;

private:
    void addState(QScxmlStateMachineInfo::StateId state);
    void addTransition(QScxmlStateMachineInfo::TransitionId transition);

    QString labelForState(QScxmlStateMachineInfo::StateId state) const
    {
        // hide the state id for the root state + invalid states
        if (state == QScxmlStateMachineInfo::InvalidStateId) {
            return m_info->stateMachine()->name();
        }

        return QStringLiteral("%1 (%2)").arg(m_info->stateName(state)).arg(state);
    }

    QString labelForTransition(QScxmlStateMachineInfo::TransitionId transition) const
    {
        if (transition == QScxmlStateMachineInfo::InvalidTransitionId) {
            return QString();
        }

        auto events = m_info->transitionEvents(transition);
        if (events.empty()) {
            return QString();
        }

        // TODO return all events combined instead of just first one
        return QStringLiteral("%1 (%2)").arg(events.first()).arg(transition);
    }

    QScopedPointer<QScxmlStateMachineInfo> m_info;
    QSet<QScxmlStateMachineInfo::StateId> m_recursionGuard;
    QSet<QScxmlStateMachineInfo::TransitionId> m_recursionGuardForTransition;
    QVector<QScxmlStateMachineInfo::StateId> m_lastStateConfig;
};

QScxmlDebugInterfaceSource::QScxmlDebugInterfaceSource()
    : d(new Private)
{
}

QScxmlDebugInterfaceSource::~QScxmlDebugInterfaceSource()
{
}

QScxmlStateMachine * QScxmlDebugInterfaceSource::qScxmlStateMachine() const
{
    return d->qScxmlStateMachine();
}

void QScxmlDebugInterfaceSource::setQScxmlStateMachine(QScxmlStateMachine* machine)
{
    d->setQScxmlStateMachine(machine);
}

QObject* QScxmlDebugInterfaceSource::remoteObjectSource() const
{
    return d.data();
}

QScxmlDebugInterfaceSource::Private::Private(QObject *parent)
    : DebugInterfaceSource(parent)
{
    DebugInterface::registerTypes();

    updateStartStop();
}

void QScxmlDebugInterfaceSource::Private::repopulateGraph()
{
    emit aboutToRepopulateGraph();

    updateStartStop();

    if (m_info) {
        // root state is not part of 'allStates', add it manually
        addState(QScxmlStateMachineInfo::InvalidStateId);

        foreach (auto stateId, m_info->allStates()) {
            addState(stateId);
        }
        foreach (auto transition, m_info->allTransitions()) {
            addTransition(transition);
        }

        m_recursionGuard.clear();
        m_recursionGuardForTransition.clear();
    }

    emit graphRepopulated();

    // make sure to pass the current config to the listener
    handleStateConfigurationChanged();
}

QScxmlStateMachine *QScxmlDebugInterfaceSource::Private::qScxmlStateMachine() const
{
    return m_info ? m_info->stateMachine() : nullptr;
}

void QScxmlDebugInterfaceSource::Private::setQScxmlStateMachine(QScxmlStateMachine* machine)
{
    repopulateGraph();

    m_info.reset();

    handleStateConfigurationChanged();

    m_info.reset(machine ? new QScxmlStateMachineInfo(machine) : nullptr);
    repopulateGraph();

    if (m_info) {
        connect(m_info.data(), &QScxmlStateMachineInfo::statesEntered, this,
                [this](const QVector<QScxmlStateMachineInfo::StateId> &states) {
            // TODO: Don't just use the first
            if (!states.isEmpty()) {
                stateEntered(states.first());
            }
        });
        connect(m_info.data(), &QScxmlStateMachineInfo::statesExited, this,
                [this](const QVector<QScxmlStateMachineInfo::StateId> &states) {
            // TODO: Don't just use the first
            if (!states.isEmpty()) {
                stateExited(states.first());
            }
        });
        connect(m_info.data(), &QScxmlStateMachineInfo::transitionsTriggered,
                this, [this](const QVector<QScxmlStateMachineInfo::TransitionId> &transitions) {
            // TODO: Don't just use the first
            if (!transitions.isEmpty()) {
                handleTransitionTriggered(transitions.first());
            }
        });
    }

    updateStartStop();
}

void QScxmlDebugInterfaceSource::Private::handleTransitionTriggered(QScxmlStateMachineInfo::TransitionId transition)
{
    emit transitionTriggered(makeTransitionId(transition), labelForTransition(transition));
}

void QScxmlDebugInterfaceSource::Private::stateEntered(QScxmlStateMachineInfo::StateId state)
{
    emit message(tr("State entered: %1").arg(labelForState(state)));
    handleStateConfigurationChanged();
}

void QScxmlDebugInterfaceSource::Private::stateExited(QScxmlStateMachineInfo::StateId state)
{
    emit message(tr("State exited: %1").arg(labelForState(state)));
    handleStateConfigurationChanged();
}

void QScxmlDebugInterfaceSource::Private::handleStateConfigurationChanged()
{
    QVector<QScxmlStateMachineInfo::StateId> newConfig;
    if (m_info) {
        newConfig = m_info->configuration();
    }

    if (newConfig == m_lastStateConfig) {
        return;
    }
    m_lastStateConfig = newConfig;

    StateMachineConfiguration config;
    config.reserve(newConfig.size());
    foreach (auto state, newConfig) {
        config << makeStateId(state);
    }

    emit stateConfigurationChanged(config);
}

void QScxmlDebugInterfaceSource::Private::addState(QScxmlStateMachineInfo::StateId state)
{
    if (m_recursionGuard.contains(state)) {
        return;
    }

    m_recursionGuard.insert(state);

    auto parentState = m_info->stateParent(state);
    addState(parentState); // be sure that parent is added first

    const auto children = m_info->stateChildren(state);
    const bool hasChildren = !children.isEmpty();

    // add a connection from parent state to initial state if
    // parent state is valid and parent state has an initial state
    const auto parentInitialTransition = m_info->initialTransition(parentState);
    if (parentInitialTransition != QScxmlStateMachineInfo::InvalidTransitionId) {
        m_recursionGuardForTransition.insert(parentInitialTransition);
    }

    const auto parentInitialTransitionTargets = m_info->transitionTargets(parentInitialTransition);
    Q_ASSERT(parentInitialTransitionTargets.size() <= 1); // assume there can only be at most one 'initial state'
    const auto parentInitialState = parentInitialTransitionTargets.value(0);
    const bool connectToInitial = parentInitialState == state; // TODO ?
    emit stateAdded(makeStateId(state), makeStateId(parentState),
                    hasChildren, labelForState(state),
                    makeStateType(m_info->stateType(state)), connectToInitial);

    // add sub-states
    Q_FOREACH (int child, m_info->stateChildren(state)) {
        addState(child);
    }
}

void QScxmlDebugInterfaceSource::Private::addTransition(QScxmlStateMachineInfo::TransitionId transition)
{
    if (m_recursionGuardForTransition.contains(transition)) {
        return;
    }

    m_recursionGuardForTransition.insert(transition);

    const auto sourceState = m_info->transitionSource(transition);
    const auto targetStates = m_info->transitionTargets(transition);
    addState(sourceState);
    foreach (int targetState, targetStates) {
        addState(targetState);
    }

    foreach (int targetState, targetStates) {
        emit transitionAdded(makeTransitionId(transition), makeStateId(sourceState),
                             makeStateId(targetState), labelForTransition(transition));
    }
}

void QScxmlDebugInterfaceSource::Private::updateStartStop()
{
    emit statusChanged(qScxmlStateMachine() != nullptr, qScxmlStateMachine() && qScxmlStateMachine()->isRunning());
}

void QScxmlDebugInterfaceSource::Private::toggleRunning()
{
    if (!qScxmlStateMachine()) {
        return;
    }

    if (qScxmlStateMachine()->isRunning()) {
        qScxmlStateMachine()->stop();
    } else {
        qScxmlStateMachine()->start();
    }
}

#include "qscxmldebuginterfacesource.moc"
