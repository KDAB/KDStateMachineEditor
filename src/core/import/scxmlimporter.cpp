/*
  scxmlimporter.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "scxmlimporter.h"

#include "debug.h"
#include "element.h"
#include "state.h"
#include "transition.h"

#include <QHash>
#include <QXmlStreamReader>

#define IF_DEBUG(x)

using namespace KDSME;

struct ScxmlImporter::Private
{
    Private(ScxmlImporter* q) : q(q) {}

    /**
     * Start point for XML parsing
     *
     * See http://www.w3.org/2011/04/SCXML/scxml-module-core.xsd for the allowed XML input
     */
    StateMachine* visitScxml();
    void visitTransiton(State* parent);
    void visitState(State* parent);
    void visitInitial(State* parent);
    void visitParallel(State* parent);
    void visitFinal(State* parent);
    void visitHistory(State* parent);

    /// Reset the parser to the initial state (clear cache, etc.)
    void reset();

    /// Create initial state based on current stream reader context
    State* tryCreateInitialState(State* parent);
    /// Initialize @p state based on current stream reader context
    void initState(State* state);

    /// Create Transition
    Transition* createTransition(State* parent, const QString& targetStateId);

    void raiseUnexpectedElementError(const QString& context);

    /// Resolves any unresolved targets in transitions
    void resolveTargetStates();

    ScxmlImporter* q;

    QXmlStreamReader m_reader;

    /// Map: Transition -> Transition target state id
    QHash<Transition*, QString> m_unresolvedTargetStateIds;
    QHash<QString, State*> m_nameToStateMap;

    QByteArray m_data;
};

ScxmlImporter::ScxmlImporter(const QByteArray& data)
    : d(new Private(this))
{
    d->m_data = data;
}

ScxmlImporter::~ScxmlImporter()
{
}

StateMachine* ScxmlImporter::import()
{
    setErrorString(QString());
    d->reset();

    if (d->m_data.isEmpty()) {
        setErrorString(tr("No data supplied"));
        return nullptr;
    }

    d->m_reader.addData(d->m_data);

    StateMachine* stateMachine = nullptr;

    if (d->m_reader.readNextStartElement() && d->m_reader.name() == "scxml") {
        stateMachine = d->visitScxml();
    } else {
        d->m_reader.raiseError(tr("This document does not start with an <scxml> element"));
    }

    if (!d->m_reader.hasError()) {
        // All states have been created by now, we can now link the transitions to their
        // resp. target states
        d->resolveTargetStates();
    }

    if (d->m_reader.hasError()) {
        // pass error string to *this
        setErrorString(d->m_reader.errorString());

        delete stateMachine;
        stateMachine = nullptr;
    }
    return stateMachine;
}

StateMachine* ScxmlImporter::Private::visitScxml()
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

void ScxmlImporter::Private::visitParallel(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "parallel");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    State* state = new State(parent);
    state->setChildMode(State::ParallelStates);
    initState(state);
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

void ScxmlImporter::Private::visitState(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "state");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    auto state = new State(parent);
    initState(state);
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

void ScxmlImporter::Private::visitInitial(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "initial");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    // Must have exactly one <transition> child
    Transition* transition = nullptr;
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

void ScxmlImporter::Private::visitFinal(State* parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "final");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    auto state = new FinalState(parent);
    initState(state);

    m_reader.skipCurrentElement();
}

void ScxmlImporter::Private::visitTransiton(State* parent)
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

void ScxmlImporter::Private::visitHistory(State* parent)
{
    Q_UNUSED(parent);
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == "transition");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    qWarning() << "NYI";

    m_reader.skipCurrentElement();
}

void ScxmlImporter::Private::resolveTargetStates()
{
    IF_DEBUG(qCDebug(KDSME_CORE) << m_nameToStateMap;)

    auto it = m_unresolvedTargetStateIds.constBegin();
    while (it != m_unresolvedTargetStateIds.constEnd()) {
        const QString targetStateId = it.value();
        State* targetState = m_nameToStateMap.value(targetStateId);
        if (!targetState) {
            m_reader.raiseError(QString("Unknown state id: %1").arg(targetStateId));
            return;
        }

        Transition* transition = it.key();
        transition->setTargetState(targetState);
        ++it;
    }
}

State* ScxmlImporter::Private::tryCreateInitialState(State* parent)
{
    const QXmlStreamAttributes attributes = m_reader.attributes();
    if (attributes.hasAttribute("initial")) {
        State* initialState = new PseudoState(PseudoState::InitialState, parent);
        const QString initialStateId = attributes.value("initial").toString();
        createTransition(initialState, initialStateId);
    }
    return nullptr;
}

void ScxmlImporter::Private::initState(State* state)
{
    Q_ASSERT(state);

    const QXmlStreamAttributes attributes = m_reader.attributes();
    const QString id = attributes.value("id").toString();
    IF_DEBUG(qCDebug(KDSME_CORE) << parent->label() << id;)
    if (id.isEmpty()) {
        qWarning() << "Unnamed state at offset:" << m_reader.characterOffset();
    }
    state->setLabel(id);
    m_nameToStateMap[id] = state;
}

Transition* ScxmlImporter::Private::createTransition(State* parent, const QString& targetStateId)
{
    IF_DEBUG(qCDebug(KDSME_CORE) << parent->label() << targetStateId);
    if (targetStateId.isEmpty()) {
        return nullptr;
    }

    Transition* transition = new Transition(parent);
    m_unresolvedTargetStateIds[transition] = targetStateId;
    return transition;
}

void ScxmlImporter::Private::reset()
{
    m_nameToStateMap.clear();
    m_unresolvedTargetStateIds.clear();
    m_reader.clear();
}

void ScxmlImporter::Private::raiseUnexpectedElementError(const QString& context)
{
    m_reader.raiseError(QString("Unexpected element found while parsing '%1': %2")
        .arg(context, m_reader.name().toString()));
}
