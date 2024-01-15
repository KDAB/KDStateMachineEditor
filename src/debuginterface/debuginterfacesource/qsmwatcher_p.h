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

#ifndef QSMWATCHER_H
#define QSMWATCHER_H

#include <QObject>
#include <QVector>

QT_BEGIN_NAMESPACE
class QAbstractState;
class QAbstractState;
class QAbstractTransition;
class QState;
class QStateMachine;
QT_END_NAMESPACE

class QSMWatcher : public QObject
{
    Q_OBJECT

public:
    explicit QSMWatcher(QObject *parent = nullptr);
    ~QSMWatcher();

    void setWatchedStateMachine(QStateMachine *machine);
    QStateMachine *watchedStateMachine() const;

Q_SIGNALS:
    void stateEntered(QAbstractState *state);
    void stateExited(QAbstractState *state);

    void transitionTriggered(QAbstractTransition *);

    void watchedStateMachineChanged(QStateMachine *);

private Q_SLOTS:
    void watchState(QAbstractState *state);
    void clearWatchedStates();

    void handleStateEntered();
    void handleStateExited();
    void handleStateDestroyed();
    void handleTransitionTriggered();

private:
    QStateMachine *m_watchedStateMachine;
    QVector<QAbstractState *> m_watchedStates;

    QAbstractState *m_lastEnteredState;
    QAbstractState *m_lastExitedState;
};

#endif // QSMWATCHER_H
