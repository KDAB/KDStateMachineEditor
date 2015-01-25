/*
  scxmlparser.cpp

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

  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Contact info@kdab.com if any conditions of this licensing are not
  clear to you.
*/

#include "scxmlparser.h"

#include "element.h"
#include "state.h"
#include "transition.h"

#include "debug.h"

#define IF_DEBUG(x)

using namespace KDSME;

ScxmlParser::ScxmlParser(QObject* parent)
    : QObject(parent)
{
}

StateMachine* ScxmlParser::parse(const QByteArray& data)
{
    reset();

    if (data.isEmpty()) {
        m_reader.raiseError(tr("No data supplied"));
        return nullptr;
    }

    m_reader.addData(data);

    StateMachine* stateMachine = 0;

    if (m_reader.readNextStartElement() && m_reader.name() == "scxml") {
        stateMachine = visitScxml();
    } else {
        m_reader.raiseError(tr("This document does not start with an <scxml> element"));
    }

    if (!m_reader.hasError()) {
        // All states have been created by now, we can now link the transitions to their
        // resp. target states
        resolveTargetStates();
    }

    if (m_reader.hasError()) {
        delete stateMachine;
        stateMachine = 0;
    }
    return stateMachine;
}

bool ScxmlParser::hasError() const
{
    return m_reader.hasError();
}

QString ScxmlParser::errorString() const
{
    return m_reader.errorString();
}

StateMachine* ScxmlParser::visitScxml()
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "scxml");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    const QXmlStreamAttributes attributes = m_reader.attributes();

    StateMachine* state = new StateMachine;
    state->setLabel(attributes.value("name").toString());

    tryCreateInitialState(state);

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == "state") {
            visitState(state);
        } else if (m_reader.name() == "parallel") {
            visitParallel(state);
        } else if (m_reader.name() == "final") {
            visitFinal(state);
        } else if (m_reader.name() == "datamodel") {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == "script") {
            m_reader.skipCurrentElement();
        } else {
            raiseUnexpectedElementError("scxml");
        }
    }
    return state;
}

void ScxmlParser::visitParallel(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "parallel");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    State* state = createState<State>(parent);
    tryCreateInitialState(state);

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == "onentry" || m_reader.name() == "onexit") {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == "transition") {
            visitTransiton(state);
        } else if (m_reader.name() == "state") {
            visitState(state);
        } else if (m_reader.name() == "parallel") {
            visitParallel(state);
        } else if (m_reader.name() == "datamodel") {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == "invoke") {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == "history" ) {
            visitHistory(state);
        } else {
            raiseUnexpectedElementError("parallel");
        }
    }
}

void ScxmlParser::visitState(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "state");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    // TODO: This could be a atomic or composite state
    // How to resolve?
    State* state = createState<State>(parent);
    tryCreateInitialState(state);

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == "onentry" || m_reader.name() == "onexit") {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == "transition") {
            visitTransiton(state);
        } else if (m_reader.name() == "initial") {
            visitInitial(state);
        } else if (m_reader.name() == "state") {
            visitState(state);
        } else if (m_reader.name() == "parallel") {
            visitParallel(state);
        } else if (m_reader.name() == "final") {
            visitFinal(state);
        } else if (m_reader.name() == "history") {
            visitHistory(state);
        } else if (m_reader.name() == "datamodel") {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == "invoke") {
            m_reader.skipCurrentElement();
        } else {
            raiseUnexpectedElementError("state");
        }
    }
}

void ScxmlParser::visitInitial(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "initial");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    // Must have exactly one <transition> child
    Transition* transition = 0;
    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == "transition") {
            State* initialState = new PseudoState(PseudoState::InitialState, parent);
            const QXmlStreamAttributes attributes = m_reader.attributes();
            const QString targetStateId = attributes.value("target").toString();
            transition = createTransition(initialState, targetStateId);
        } else {
           raiseUnexpectedElementError("initial");
        }
    }
    if (!transition) {
        m_reader.raiseError(QString("Encountered <initial> element with invalid <transition> child"));
    }

    m_reader.skipCurrentElement();

}

void ScxmlParser::visitFinal(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "final");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    createState<FinalState>(parent);

    m_reader.skipCurrentElement();
}

void ScxmlParser::visitTransiton(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "transition");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    const QXmlStreamAttributes attributes = m_reader.attributes();
    const QString event = attributes.value("event").toString();
    const QString targetStateId = attributes.value("target").toString();
    Transition* transition = createTransition(parent, targetStateId);
    if (transition) {
        transition->setLabel(event);
    }

    m_reader.skipCurrentElement();
}

void ScxmlParser::visitHistory(State* parent)
{
    Q_UNUSED(parent);
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "transition");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    qWarning() << "NYI";

    m_reader.skipCurrentElement();
}

void ScxmlParser::resolveTargetStates()
{
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO << m_nameToStateMap;)

    auto it = m_unresolvedTargetStateIds.constBegin();
    while (it != m_unresolvedTargetStateIds.constEnd()) {
        const QString targetStateId = it.value();
        State* targetState = m_nameToStateMap.value(targetStateId);
        if (!targetState) {
            raiseError(QString("Unknown state id: %1").arg(targetStateId));
            return;
        }

        Transition* transition = it.key();
        transition->setTargetState(targetState);
        ++it;
    }
}

State* ScxmlParser::tryCreateInitialState(State* parent)
{
    const QXmlStreamAttributes attributes = m_reader.attributes();
    if (attributes.hasAttribute("initial")) {
        State* initialState = new PseudoState(PseudoState::InitialState, parent);
        const QString initialStateId = attributes.value("initial").toString();
        createTransition(initialState, initialStateId);
    }
    return nullptr;
}

// TODO: Error handling
template<typename T>
T* ScxmlParser::createState(State* parent)
{
    const QXmlStreamAttributes attributes = m_reader.attributes();
    const QString id = attributes.value("id").toString();
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO << parent->label() << id;)
    if (id.isEmpty()) {
        qWarning() << "Unnamed state at offset:" << m_reader.characterOffset();
    }
    T* state = new T(parent);
    state->setLabel(id);
    m_nameToStateMap[id] = state;
    return state;
}

Transition* ScxmlParser::createTransition(State* parent, const QString& targetStateId)
{
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO << parent->label() << targetStateId);
    if (targetStateId.isEmpty()) {
        return nullptr;
    }

    Transition* transition = new Transition(parent);
    m_unresolvedTargetStateIds[transition] = targetStateId;
    return transition;
}

void ScxmlParser::reset()
{
    m_nameToStateMap.clear();
    m_unresolvedTargetStateIds.clear();
    m_reader.clear();
}

void ScxmlParser::raiseError(const QString& message)
{
    m_reader.raiseError(message);
}

void ScxmlParser::raiseUnexpectedElementError(const QString& context)
{
    m_reader.raiseError(QString("Unexpected element found while parsing '%1': %2")
        .arg(context, m_reader.name().toString()));
}

#include "moc_scxmlparser.cpp"
