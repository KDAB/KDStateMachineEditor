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

#include "runtimecontroller.h"

#include "ringbuffer.h"

using namespace KDSME;

namespace {

template<typename ContainerType, typename ItemType>
inline qreal relativePosition(const ContainerType &list, const ItemType &t)
{
    const int index = list.indexOf(t);
    return (index + 1.0) / list.size();
}

}

struct RuntimeController::Private
{
    explicit Private(RuntimeController *qq)
        : q(qq)
        , m_lastConfigurations(5)
        , m_lastTransitions(5)
        , m_isRunning(false)
    {
    }

    void updateActiveRegion();

    RuntimeController *q;

    RingBuffer<Configuration> m_lastConfigurations;
    RingBuffer<Transition *> m_lastTransitions;
    bool m_isRunning;
    QRectF m_activeRegion;
};

void RuntimeController::Private::updateActiveRegion()
{
    const Configuration configuration = q->activeConfiguration();

    // Calculate the bounding rect of all states in that are currently active
    QRectF activeRegion;
    for (const State *state : configuration) {
        activeRegion = activeRegion.united(state->boundingRect());
    }
    m_activeRegion = activeRegion;
    Q_EMIT q->activeRegionChanged(m_activeRegion);
}

RuntimeController::RuntimeController(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    qRegisterMetaType<QSet<State *>>();
}

RuntimeController::~RuntimeController()
{
}

int RuntimeController::historySize() const
{
    return d->m_lastConfigurations.size();
}

void RuntimeController::setHistorySize(int size)
{
    d->m_lastConfigurations.setCapacity(size);
    d->m_lastTransitions.setCapacity(size);
}

QRectF RuntimeController::activeRegion() const
{
    return d->m_activeRegion;
}

void RuntimeController::clear()
{
    d->m_lastConfigurations.clear();
    d->m_lastTransitions.clear();
}

RuntimeController::Configuration RuntimeController::activeConfiguration() const
{
    return (d->m_lastConfigurations.size() > 0 ? d->m_lastConfigurations.last() : Configuration());
}

QList<RuntimeController::Configuration> RuntimeController::lastConfigurations() const
{
    return d->m_lastConfigurations.entries();
}

void RuntimeController::setActiveConfiguration(const RuntimeController::Configuration &configuration)
{
    if (d->m_lastConfigurations.size() > 0 && d->m_lastConfigurations.last() == configuration)
        return;

    d->m_lastConfigurations.enqueue(configuration);
    Q_EMIT activeConfigurationChanged(configuration);
    d->updateActiveRegion();
}

QList<Transition *> RuntimeController::lastTransitions() const
{
    return d->m_lastTransitions.entries();
}

Transition *RuntimeController::lastTransition() const
{
    return (d->m_lastTransitions.size() > 0 ? d->m_lastTransitions.last() : nullptr);
}

void RuntimeController::setLastTransition(Transition *transition)
{
    if (!transition)
        return;

    d->m_lastTransitions.enqueue(transition);
}

bool RuntimeController::isRunning() const
{
    return d->m_isRunning;
}

void RuntimeController::setIsRunning(bool isRunning)
{
    if (d->m_isRunning == isRunning)
        return;

    d->m_isRunning = isRunning;
    Q_EMIT isRunningChanged(d->m_isRunning);
}

float RuntimeController::activenessForState(State *state) const
{
    const int count = d->m_lastConfigurations.size();
    for (int i = d->m_lastConfigurations.size() - 1; i >= 0; --i) {
        if (d->m_lastConfigurations.at(i).contains(state)) {
            return static_cast<float>((i + 1.) / count);
        }
    }
    return 0.;
}

float RuntimeController::activenessForTransition(Transition *transition)
{
    return static_cast<float>(relativePosition<QList<Transition *>>(d->m_lastTransitions.entries(), transition));
}
