/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "transition.h"

#include "elementutil.h"
#include "state.h"

#include <QDebug>
#include <QPainterPath>
#include <QQmlEngine>

using namespace KDSME;

struct Transition::Private
{
    Private()
        : m_targetState(nullptr)
    {
    }

    State *m_targetState;
    QString m_guard;
    QPainterPath m_shape;
    QRectF m_labelBoundingRect;
};

Transition::Transition(State *sourceState)
    : Element(sourceState)
    , d(new Private)
{
}

Transition::~Transition()
{
}

StateMachine *Transition::machine() const
{
    StateMachine *m = ElementUtil::findStateMachine(this);
    QQmlEngine::setObjectOwnership(m, QQmlEngine::CppOwnership);
    return m;
}

State *Transition::sourceState() const
{
    return qobject_cast<State *>(parent());
}

void Transition::setSourceState(State *sourceState)
{
    if (parent() == sourceState) {
        return;
    }

    setParent(sourceState);
    Q_EMIT sourceStateChanged(sourceState);
}

State *Transition::targetState() const
{
    return d->m_targetState;
}

void Transition::setTargetState(State *targetState)
{
    if (d->m_targetState == targetState)
        return;

    d->m_targetState = targetState;
    Q_EMIT targetStateChanged(targetState);
}

Element::Type Transition::type() const
{
    return TransitionType;
}

QString Transition::guard() const
{
    return d->m_guard;
}

void Transition::setGuard(const QString &guard)
{
    if (d->m_guard == guard)
        return;

    d->m_guard = guard;
    Q_EMIT guardChanged(d->m_guard);
}

QPainterPath Transition::shape() const
{
    return d->m_shape;
}

void Transition::setShape(const QPainterPath &shape)
{
    if (d->m_shape == shape)
        return;

    d->m_shape = shape;
    Q_EMIT shapeChanged(shape);
}

QRectF Transition::labelBoundingRect() const
{
    return d->m_labelBoundingRect;
}

void Transition::setLabelBoundingRect(const QRectF &rect)
{
    if (d->m_labelBoundingRect == rect)
        return;

    d->m_labelBoundingRect = rect;
    Q_EMIT labelBoundingRectChanged(rect);
}

struct SignalTransition::Private
{
    QString m_signal;
};

SignalTransition::SignalTransition(State *sourceState)
    : Transition(sourceState)
    , d(new Private)
{
}

SignalTransition::~SignalTransition()
{
}

Element::Type SignalTransition::type() const
{
    return SignalTransitionType;
}

QString SignalTransition::signal() const
{
    return d->m_signal;
}

void SignalTransition::setSignal(const QString &signal)
{
    if (d->m_signal == signal)
        return;

    d->m_signal = signal;
    Q_EMIT signalChanged(d->m_signal);
}

struct TimeoutTransition::Private
{
    Private()
        : m_timeout(0)
    {
    }

    int m_timeout;
};

TimeoutTransition::TimeoutTransition(State *sourceState)
    : Transition(sourceState)
    , d(new Private)
{
}

TimeoutTransition::~TimeoutTransition()
{
}

Element::Type TimeoutTransition::type() const
{
    return TimeoutTransitionType;
}

int TimeoutTransition::timeout() const
{
    return d->m_timeout;
}

void TimeoutTransition::setTimeout(int timeout)
{
    if (d->m_timeout == timeout)
        return;

    d->m_timeout = timeout;
    Q_EMIT timeoutChanged(d->m_timeout);
}

QDebug KDSME::operator<<(QDebug dbg, const Transition *transition)
{
    if (!transition) {
        return dbg << static_cast<QObject *>(nullptr);
    }
    dbg.nospace() << "Transition["
                  << "this=" << ( const void * )transition
                  << ", source=" << transition->sourceState()
                  << ", target=" << transition->targetState()
                  << "]";
    return dbg.space();
}

#include "moc_transition.cpp"
