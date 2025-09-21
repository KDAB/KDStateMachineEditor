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

#include "qsmdebuginterfacesource.h"

#include <config-kdsme.h>

#include "rep_debuginterface_source.h"

#include "qsmwatcher_p.h"

#include "objecthelper.h"

#include <QAbstractTransition>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QStateMachine>

using namespace KDSME;
using namespace DebugInterface;
using namespace std;

namespace {

StateId makeStateId(QAbstractState *state)
{
    return StateId { reinterpret_cast<quint64>(state) };
}
TransitionId makeTransitionId(QAbstractTransition *transition)
{
    return TransitionId { reinterpret_cast<quint64>(transition) };
}

QString labelForTransition(QAbstractTransition *transition)
{
    const QString objectName = transition->objectName();
    if (!objectName.isEmpty()) {
        return objectName;
    }

    // Try to get a label for the transition if it is a QSignalTransition.
    QSignalTransition *signalTransition = qobject_cast<QSignalTransition *>(transition);
    if (signalTransition) {
        return QString::fromLatin1("%1::%2").arg(ObjectHelper::displayString(signalTransition->senderObject())).arg(QString::fromLatin1(signalTransition->signal().mid(1)));
    }

    return ObjectHelper::displayString(transition);
}

}

class QsmDebugInterfaceSource::Private : public DebugInterfaceSource
{
    Q_OBJECT

public:
    explicit Private(QObject *parent = nullptr);

    void addState(QAbstractState *state);
    void addTransition(QAbstractTransition *transition);

    QStateMachine *qStateMachine() const;
    void setQStateMachine(QStateMachine *machine);

private Q_SLOTS:
    void stateEntered(QAbstractState *state);
    void stateExited(QAbstractState *state);
    void handleStateConfigurationChanged();
    void handleTransitionTriggered(QAbstractTransition *);

    void updateStartStop();
    void toggleRunning();

    void repopulateGraph() override;

private:
    void updateStateItems();

    bool mayAddState(QAbstractState *state);

    QSMWatcher *m_stateMachineWatcher;
    QSet<QAbstractState *> m_recursionGuard;
    QSet<QAbstractState *> m_lastStateConfig;
};

QsmDebugInterfaceSource::QsmDebugInterfaceSource()
    : d(new Private)
{
}

QsmDebugInterfaceSource::~QsmDebugInterfaceSource()
{
}

QStateMachine *QsmDebugInterfaceSource::qStateMachine() const
{
    return d->qStateMachine();
}

void QsmDebugInterfaceSource::setQStateMachine(QStateMachine *machine)
{
    d->setQStateMachine(machine);
}

QObject *QsmDebugInterfaceSource::remoteObjectSource() const
{
    return d.data();
}

QsmDebugInterfaceSource::Private::Private(QObject *parent)
    : DebugInterfaceSource(parent)
    , m_stateMachineWatcher(new QSMWatcher(this))
{
    DebugInterface::registerTypes();

    connect(m_stateMachineWatcher, SIGNAL(stateEntered(QAbstractState *)),
            SLOT(stateEntered(QAbstractState *)));
    connect(m_stateMachineWatcher, SIGNAL(stateExited(QAbstractState *)),
            SLOT(stateExited(QAbstractState *)));
    connect(m_stateMachineWatcher, SIGNAL(transitionTriggered(QAbstractTransition *)),
            SLOT(handleTransitionTriggered(QAbstractTransition *)));

    updateStartStop();
}

void QsmDebugInterfaceSource::Private::repopulateGraph()
{
    Q_EMIT aboutToRepopulateGraph();

    updateStartStop();

    addState(qStateMachine());
    m_recursionGuard.clear();

    Q_EMIT graphRepopulated();

    // make sure to pass the current config to the listener
    handleStateConfigurationChanged();
}

QStateMachine *QsmDebugInterfaceSource::Private::qStateMachine() const
{
    return m_stateMachineWatcher->watchedStateMachine();
}

bool QsmDebugInterfaceSource::Private::mayAddState(QAbstractState *state)
{
    if (!state) {
        return false;
    }

    if (m_recursionGuard.contains(state)) {
        return false;
    }
    return true;
}

void QsmDebugInterfaceSource::Private::setQStateMachine(QStateMachine *machine)
{
    QStateMachine *oldMachine = qStateMachine();
    if (oldMachine) {
        disconnect(oldMachine, SIGNAL(started()), this, SLOT(updateStartStop()));
        disconnect(oldMachine, SIGNAL(stopped()), this, SLOT(updateStartStop()));
        disconnect(oldMachine, SIGNAL(finished()), this, SLOT(updateStartStop()));
    }

    // m_stateModel->setStateMachine(machine);
    handleStateConfigurationChanged();

    m_stateMachineWatcher->setWatchedStateMachine(machine);
    repopulateGraph();

    if (machine) {
        connect(machine, SIGNAL(started()), this, SLOT(updateStartStop()));
        connect(machine, SIGNAL(stopped()), this, SLOT(updateStartStop()));
        connect(machine, SIGNAL(finished()), this, SLOT(updateStartStop()));
    }
    updateStartStop();
}

void QsmDebugInterfaceSource::Private::handleTransitionTriggered(QAbstractTransition *transition)
{
    Q_EMIT transitionTriggered(makeTransitionId(transition), ObjectHelper::displayString(transition));
}

void QsmDebugInterfaceSource::Private::stateEntered(QAbstractState *state)
{
    Q_EMIT message(tr("State entered: %1").arg(ObjectHelper::displayString(state)));
    handleStateConfigurationChanged();
}

void QsmDebugInterfaceSource::Private::stateExited(QAbstractState *state)
{
    Q_EMIT message(tr("State exited: %1").arg(ObjectHelper::displayString(state)));
    handleStateConfigurationChanged();
}

void QsmDebugInterfaceSource::Private::handleStateConfigurationChanged()
{
    QSet<QAbstractState *> newConfig;
    if (qStateMachine()) {
        newConfig = qStateMachine()->configuration();
    }

    if (newConfig == m_lastStateConfig) {
        return;
    }
    m_lastStateConfig = newConfig;

    StateMachineConfiguration config;
    config.reserve(newConfig.size());
    for (QAbstractState *state : std::as_const(newConfig)) {
        config << makeStateId(state);
    }

    Q_EMIT stateConfigurationChanged(config);
}

void QsmDebugInterfaceSource::Private::addState(QAbstractState *state)
{
    if (!state)
        return;

    if (!mayAddState(state)) {
        return;
    }

    Q_ASSERT(!m_recursionGuard.contains(state));
    m_recursionGuard.insert(state);

    QState *parentState = state->parentState();
    if (parentState) {
        addState(parentState); // be sure that parent is added first
    }

    const bool hasChildren = state->findChild<QAbstractState *>();
    const QString &label = ObjectHelper::displayString(state);
    // add a connection from parent state to initial state if
    // parent state is valid and parent state has an initial state
    const bool connectToInitial = parentState && parentState->initialState() == state;
    StateType type = OtherState;
    if (qobject_cast<QFinalState *>(state)) {
        type = FinalState;
    } else if (auto historyState = qobject_cast<QHistoryState *>(state)) {
        type = historyState->historyType() == QHistoryState::ShallowHistory ? ShallowHistoryState : DeepHistoryState;
    } else if (qobject_cast<QStateMachine *>(state)) {
        type = StateMachineState;
    }

    Q_EMIT stateAdded(makeStateId(state), makeStateId(parentState),
                      hasChildren, label, type, connectToInitial);

    // add outgoing transitions
    Q_FOREACH (auto transition, state->findChildren<QAbstractTransition *>(QString(), Qt::FindDirectChildrenOnly)) {
        addTransition(transition);
    }

    // add sub-states
    Q_FOREACH (auto child, state->findChildren<QAbstractState *>(QString(), Qt::FindDirectChildrenOnly)) {
        addState(child);
    }
}

void QsmDebugInterfaceSource::Private::addTransition(QAbstractTransition *transition)
{
    QState *sourceState = transition->sourceState();
    QAbstractState *targetState = transition->targetState();
    addState(sourceState);
    addState(targetState);

    const QString label = labelForTransition(transition);
    Q_EMIT transitionAdded(makeTransitionId(transition), makeStateId(sourceState),
                           makeStateId(targetState), label);
}

void QsmDebugInterfaceSource::Private::updateStartStop()
{
    Q_EMIT statusChanged(qStateMachine() != nullptr, qStateMachine() && qStateMachine()->isRunning());
}

void QsmDebugInterfaceSource::Private::toggleRunning()
{
    if (!qStateMachine()) {
        return;
    }

    if (qStateMachine()->isRunning()) {
        qStateMachine()->stop();
    } else {
        qStateMachine()->start();
    }
}

#include "qsmdebuginterfacesource.moc"
