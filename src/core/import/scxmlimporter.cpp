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
    explicit Private(ScxmlImporter *q)
        : q(q)
    {
    }

    /**
     * Start point for XML parsing
     *
     * See http://www.w3.org/2011/04/SCXML/scxml-module-core.xsd for the allowed XML input
     */
    StateMachine *visitScxml();
    void visitTransiton(State *parent);
    void visitState(State *parent);
    void visitInitial(State *parent);
    void visitParallel(State *parent);
    void visitFinal(State *parent);
    void visitHistory(State *parent);

    /// Reset the parser to the initial state (clear cache, etc.)
    void reset();

    /// Create initial state based on current stream reader context
    State *tryCreateInitialState(State *parent);
    /// Initialize @p state based on current stream reader context
    void initState(State *state);

    /// Create Transition
    Transition *createTransition(State *parent, const QString &targetStateId);

    void raiseUnexpectedElementError(const QString &context);

    /// Resolves any unresolved targets in transitions
    void resolveTargetStates();

    ScxmlImporter *q;

    QXmlStreamReader m_reader;

    /// Map: Transition -> Transition target state id
    QHash<Transition *, QString> m_unresolvedTargetStateIds;
    QHash<QString, State *> m_nameToStateMap;

    QByteArray m_data;
};

ScxmlImporter::ScxmlImporter(const QByteArray &data)
    : d(new Private(this))
{
    d->m_data = data;
}

ScxmlImporter::~ScxmlImporter()
{
}

StateMachine *ScxmlImporter::import()
{
    setErrorString(QString());
    d->reset();

    if (d->m_data.isEmpty()) {
        setErrorString(tr("No data supplied"));
        return nullptr;
    }

    d->m_reader.addData(d->m_data);

    StateMachine *stateMachine = nullptr;

    if (d->m_reader.readNextStartElement() && d->m_reader.name() == QStringLiteral("scxml")) {
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

StateMachine *ScxmlImporter::Private::visitScxml()
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QStringLiteral("scxml"));
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    const QXmlStreamAttributes attributes = m_reader.attributes();

    auto *state = new StateMachine;
    state->setLabel(attributes.value(QStringLiteral("name")).toString());

    tryCreateInitialState(state);

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == QStringLiteral("state")) {
            visitState(state);
        } else if (m_reader.name() == QStringLiteral("parallel")) {
            visitParallel(state);
        } else if (m_reader.name() == QStringLiteral("final")) {
            visitFinal(state);
        } else if (m_reader.name() == QStringLiteral("datamodel")) {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == QStringLiteral("script")) {
            m_reader.skipCurrentElement();
        } else {
            raiseUnexpectedElementError(QStringLiteral("scxml"));
        }
    }
    return state;
}

void ScxmlImporter::Private::visitParallel(State *parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QStringLiteral("parallel"));
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    auto *state = new State(parent);
    state->setChildMode(State::ParallelStates);
    initState(state);
    tryCreateInitialState(state);

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == QStringLiteral("onentry") || m_reader.name() == QStringLiteral("onexit")) {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == QStringLiteral("transition")) {
            visitTransiton(state);
        } else if (m_reader.name() == QStringLiteral("state")) {
            visitState(state);
        } else if (m_reader.name() == QStringLiteral("parallel")) {
            visitParallel(state);
        } else if (m_reader.name() == QStringLiteral("datamodel")) {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == QStringLiteral("invoke")) {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == QStringLiteral("history")) {
            visitHistory(state);
        } else {
            raiseUnexpectedElementError(QStringLiteral("parallel"));
        }
    }
}

void ScxmlImporter::Private::visitState(State *parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QStringLiteral("state"));
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    auto state = new State(parent);
    initState(state);
    tryCreateInitialState(state);

    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == QStringLiteral("onentry") || m_reader.name() == QStringLiteral("onexit")) {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == QStringLiteral("transition")) {
            visitTransiton(state);
        } else if (m_reader.name() == QStringLiteral("initial")) {
            visitInitial(state);
        } else if (m_reader.name() == QStringLiteral("state")) {
            visitState(state);
        } else if (m_reader.name() == QStringLiteral("parallel")) {
            visitParallel(state);
        } else if (m_reader.name() == QStringLiteral("final")) {
            visitFinal(state);
        } else if (m_reader.name() == QStringLiteral("history")) {
            visitHistory(state);
        } else if (m_reader.name() == QStringLiteral("datamodel")) {
            m_reader.skipCurrentElement();
        } else if (m_reader.name() == QStringLiteral("invoke")) {
            m_reader.skipCurrentElement();
        } else {
            raiseUnexpectedElementError(QStringLiteral("state"));
        }
    }
}

void ScxmlImporter::Private::visitInitial(State *parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QStringLiteral("initial"));
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    // Must have exactly one <transition> child
    const Transition *transition = nullptr;
    while (m_reader.readNextStartElement()) {
        if (m_reader.name() == u"transition") {
            State *initialState = new PseudoState(PseudoState::InitialState, parent);
            const QXmlStreamAttributes attributes = m_reader.attributes();
            const QString targetStateId = attributes.value(QStringLiteral("target")).toString();
            transition = createTransition(initialState, targetStateId);
        } else {
            raiseUnexpectedElementError(QStringLiteral("initial"));
        }
    }
    if (!transition) {
        m_reader.raiseError(QStringLiteral("Encountered <initial> element with invalid <transition> child"));
    }

    m_reader.skipCurrentElement();
}

void ScxmlImporter::Private::visitFinal(State *parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QStringLiteral("final"));
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    auto state = new FinalState(parent);
    initState(state);

    m_reader.skipCurrentElement();
}

void ScxmlImporter::Private::visitTransiton(State *parent)
{
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QStringLiteral("transition"));
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    const QXmlStreamAttributes attributes = m_reader.attributes();
    const QString event = attributes.value(QLatin1String("event")).toString();
    const QString targetStateId = attributes.value(QLatin1String("target")).toString();
    Transition *transition = createTransition(parent, targetStateId);
    if (transition) {
        transition->setLabel(event);
    }

    m_reader.skipCurrentElement();
}

void ScxmlImporter::Private::visitHistory(State *parent)
{
    Q_UNUSED(parent);
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == u"transition");
    IF_DEBUG(qCDebug(KDSME_CORE) << Q_FUNC_INFO;)

    qCWarning(KDSME_CORE) << "NYI";

    m_reader.skipCurrentElement();
}

void ScxmlImporter::Private::resolveTargetStates()
{
    IF_DEBUG(qCDebug(KDSME_CORE) << m_nameToStateMap;)

    auto it = m_unresolvedTargetStateIds.constBegin();
    while (it != m_unresolvedTargetStateIds.constEnd()) {
        const QString targetStateId = it.value();
        State *targetState = m_nameToStateMap.value(targetStateId);
        if (!targetState) {
            m_reader.raiseError(QStringLiteral("Unknown state id: %1").arg(targetStateId));
            return;
        }

        Transition *transition = it.key();
        transition->setTargetState(targetState);
        ++it;
    }
}

State *ScxmlImporter::Private::tryCreateInitialState(State *parent)
{
    const QXmlStreamAttributes attributes = m_reader.attributes();
    if (attributes.hasAttribute(QStringLiteral("initial"))) {
        State *initialState = new PseudoState(PseudoState::InitialState, parent);
        const QString initialStateId = attributes.value(QStringLiteral("initial")).toString();
        createTransition(initialState, initialStateId);
    }
    return nullptr;
}

void ScxmlImporter::Private::initState(State *state)
{
    Q_ASSERT(state);

    const QXmlStreamAttributes attributes = m_reader.attributes();
    const QString id = attributes.value(QStringLiteral("id")).toString();
    IF_DEBUG(qCDebug(KDSME_CORE) << parent->label() << id;)
    if (id.isEmpty()) {
        qCWarning(KDSME_CORE) << "Unnamed state at offset:" << m_reader.characterOffset();
    }
    state->setLabel(id);
    m_nameToStateMap[id] = state;
}

Transition *ScxmlImporter::Private::createTransition(State *parent, const QString &targetStateId)
{
    IF_DEBUG(qCDebug(KDSME_CORE) << parent->label() << targetStateId);
    if (targetStateId.isEmpty()) {
        return nullptr;
    }

    auto *transition = new Transition(parent);
    m_unresolvedTargetStateIds[transition] = targetStateId;
    return transition;
}

void ScxmlImporter::Private::reset()
{
    m_nameToStateMap.clear();
    m_unresolvedTargetStateIds.clear();
    m_reader.clear();
}

void ScxmlImporter::Private::raiseUnexpectedElementError(const QString &context)
{
    m_reader.raiseError(QStringLiteral("Unexpected element found while parsing '%1': %2")
                            .arg(context, m_reader.name().toString()));
}
