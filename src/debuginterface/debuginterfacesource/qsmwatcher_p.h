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

    void transitionTriggered(QAbstractTransition*);

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
