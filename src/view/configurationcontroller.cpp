/*
  configurationcontroller.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "configurationcontroller.h"

#include "element.h"
#include "layoutitem.h"
#include "view.h"

using namespace KDSME;

namespace {

template<typename ContainerType, typename ItemType>
inline qreal relativePosition(const ContainerType& list, const ItemType& t)
{
    const int index = list.indexOf(t);
    return (index+1.0) / list.size();
}

}

struct ConfigurationController::Private
{
    Private(ConfigurationController* qq)
        : q(qq)
        , m_view(nullptr)
        , m_lastConfigurations(5)
        , m_lastTransitions(5)
        , m_isRunning(false)
    {}

    void updateActiveRegion();

    ConfigurationController* q;
    View* m_view;

    RingBuffer<Configuration> m_lastConfigurations;
    RingBuffer<Transition*> m_lastTransitions;
    bool m_isRunning;
    QRectF m_activeRegion;
};

void ConfigurationController::Private::updateActiveRegion()
{
    Configuration configuration = q->activeConfiguration();

    // Calculate the bounding rect of all states in that are currently active
    QRectF activeRegion;
    foreach (State* state, configuration) {
        LayoutItem* item = m_view->layoutItemForElement(state);
        if (item) {
            activeRegion = activeRegion.united(item->boundingRect());
        }
    }
    m_activeRegion = activeRegion;
    emit q->activeRegionChanged(m_activeRegion);
}

ConfigurationController::ConfigurationController(StateMachineView* parent)
    : AbstractController(parent)
    , d(new Private(this))
{
    qRegisterMetaType<QSet<State*>>();
}

ConfigurationController::~ConfigurationController()
{
}

int ConfigurationController::historySize() const
{
    return d->m_lastConfigurations.size();
}

void ConfigurationController::setHistorySize(int size)
{
    d->m_lastConfigurations.setCapacity(size);
    d->m_lastTransitions.setCapacity(size);
}

QRectF ConfigurationController::activeRegion() const
{
    return d->m_activeRegion;
}

void ConfigurationController::clear()
{
    d->m_lastConfigurations.clear();
    d->m_lastTransitions.clear();
}

ConfigurationController::Configuration ConfigurationController::activeConfiguration() const
{
    return (d->m_lastConfigurations.size() > 0 ? d->m_lastConfigurations.last() : Configuration());
}

QList<ConfigurationController::Configuration> ConfigurationController::lastConfigurations() const
{
    return d->m_lastConfigurations.entries();
}

void ConfigurationController::setActiveConfiguration(const ConfigurationController::Configuration& configuration)
{
    if (d->m_lastConfigurations.size() > 0 && d->m_lastConfigurations.last() == configuration)
        return;

    d->m_lastConfigurations.enqueue(configuration);
    emit activeConfigurationChanged(configuration);
    d->updateActiveRegion();
}

QList<Transition*> ConfigurationController::lastTransitions() const
{
    return d->m_lastTransitions.entries();
}

Transition* ConfigurationController::lastTransition() const
{
    return (d->m_lastTransitions.size() > 0 ? d->m_lastTransitions.last() : nullptr);
}

void ConfigurationController::setLastTransition(Transition* transition)
{
    if (!transition)
        return;

    d->m_lastTransitions.enqueue(transition);
}

bool ConfigurationController::isRunning() const
{
    return d->m_isRunning;
}

void ConfigurationController::setIsRunning(bool isRunning)
{
    if (d->m_isRunning == isRunning)
        return;

    d->m_isRunning = isRunning;
    emit isRunningChanged(d->m_isRunning);
}

float ConfigurationController::activenessForState(State* state) const
{
    const int count = d->m_lastConfigurations.size();
    for (int i = d->m_lastConfigurations.size()-1; i >= 0; --i) {
        if (d->m_lastConfigurations.at(i).contains(state)) {
            return (i+1.)/count;
        }
    }
    return 0.;
}

float ConfigurationController::activenessForTransition(Transition* transition)
{
    return relativePosition<QList<Transition*>>(d->m_lastTransitions.entries(), transition);
}
