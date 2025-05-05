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

#include "qsmwatcher_p.h"

#include <QAbstractTransition>
#include <QFinalState>
#include <QState>
#include <QStateMachine>

QSMWatcher::QSMWatcher(QObject *parent)
    : QObject(parent)
    , m_watchedStateMachine(nullptr)
    , m_lastEnteredState(nullptr)
    , m_lastExitedState(nullptr)
{
}

QSMWatcher::~QSMWatcher()
{
}

void QSMWatcher::setWatchedStateMachine(QStateMachine *machine)
{
    if (m_watchedStateMachine == machine) {
        return;
    }

    m_watchedStateMachine = machine;

    clearWatchedStates();
    const auto children = machine->findChildren<QAbstractState *>();
    for (QAbstractState *state : children) {
        watchState(state);
    }

    Q_EMIT watchedStateMachineChanged(machine);
}

QStateMachine *QSMWatcher::watchedStateMachine() const
{
    return m_watchedStateMachine;
}

void QSMWatcher::watchState(QAbstractState *state)
{
    if (state->machine() != m_watchedStateMachine) {
        return;
    }

    connect(state, SIGNAL(entered()),
            this, SLOT(handleStateEntered()), Qt::UniqueConnection);
    connect(state, SIGNAL(exited()),
            this, SLOT(handleStateExited()), Qt::UniqueConnection);
    connect(state, SIGNAL(destroyed(QObject *)),
            this, SLOT(handleStateDestroyed()), Qt::UniqueConnection);

    Q_FOREACH (QAbstractTransition *transition, state->findChildren<QAbstractTransition *>()) {
        connect(transition, SIGNAL(triggered()),
                this, SLOT(handleTransitionTriggered()), Qt::UniqueConnection);
    }
    m_watchedStates << state;
}

void QSMWatcher::clearWatchedStates()
{
    Q_FOREACH (QAbstractState *state, m_watchedStates) {
        disconnect(state, SIGNAL(entered()), this, SLOT(handleStateEntered()));
        disconnect(state, SIGNAL(exited()), this, SLOT(handleStateExited()));
        disconnect(state, SIGNAL(destroyed(QObject *)), this, SLOT(handleStateDestroyed()));

        Q_FOREACH (QAbstractTransition *transition, state->findChildren<QAbstractTransition *>()) {
            disconnect(transition, SIGNAL(triggered()), this, SLOT(handleTransitionTriggered()));
        }
    }
    m_watchedStates.clear();
}

void QSMWatcher::handleTransitionTriggered()
{
    QAbstractTransition *transition = qobject_cast<QAbstractTransition *>(QObject::sender());
    Q_ASSERT(transition);

    Q_EMIT transitionTriggered(transition);
}

void QSMWatcher::handleStateEntered()
{
    QAbstractState *state = qobject_cast<QAbstractState *>(QObject::sender());
    Q_ASSERT(state);
    if (state->machine() != m_watchedStateMachine) {
        return;
    }

    if (state == m_lastEnteredState) {
        return;
    }

    m_lastEnteredState = state;
    Q_EMIT stateEntered(state);
}

void QSMWatcher::handleStateExited()
{
    QAbstractState *state = qobject_cast<QAbstractState *>(QObject::sender());
    Q_ASSERT(state);

    if (state->machine() != m_watchedStateMachine) {
        return;
    }

    if (state == m_lastExitedState) {
        return;
    }

    m_lastExitedState = state;
    Q_EMIT stateExited(state);
}

void QSMWatcher::handleStateDestroyed()
{
    QAbstractState *state = static_cast<QAbstractState *>(QObject::sender());
    Q_ASSERT(state);

    const int index = m_watchedStates.indexOf(state);
    Q_ASSERT(index != -1);
    m_watchedStates.remove(index);
}
