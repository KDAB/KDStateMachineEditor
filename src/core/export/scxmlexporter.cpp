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

#include "scxmlexporter.h"

#include "objecthelper.h"
#include "element.h"
#include "elementutil.h"
#include "state.h"
#include "transition.h"

#include "debug.h"
#include <QXmlStreamWriter>

using namespace KDSME;

struct ScxmlExporter::Private
{
    Private(QByteArray *array, ScxmlExporter *q);
    Private(QIODevice *device, ScxmlExporter *q);

    void init();

    bool writeStateMachine(StateMachine *machine);
    bool writeState(State *state);
    bool writeStateInner(State *state);
    bool writeTransition(const Transition *transition);

    ScxmlExporter *q;
    QXmlStreamWriter m_writer;
};

ScxmlExporter::Private::Private(QByteArray *array, ScxmlExporter *q)
    : q(q)
    , m_writer(array)
{
    init();
}

ScxmlExporter::Private::Private(QIODevice *device, ScxmlExporter *q)
    : q(q)
    , m_writer(device)
{
    init();
}

ScxmlExporter::ScxmlExporter(QByteArray *array)
    : d(new Private(array, this))
{
}

ScxmlExporter::ScxmlExporter(QIODevice *device)
    : d(new Private(device, this))
{
}

ScxmlExporter::~ScxmlExporter()
{
}

void ScxmlExporter::Private::init()
{
    m_writer.setAutoFormatting(true);
}

bool ScxmlExporter::exportMachine(StateMachine *machine)
{
    setErrorString(QString());

    if (!machine) {
        setErrorString(QStringLiteral("Null machine instance passed"));
        return false;
    }

    if (d->m_writer.hasError()) {
        setErrorString(QStringLiteral("Setting up XML writer failed"));
        return false;
    }

    return d->writeStateMachine(machine);
}

bool ScxmlExporter::Private::writeStateMachine(StateMachine *machine)
{
    Q_ASSERT(machine);


    // TODO: Check if preconditions are met, e.g. that all state labels are unique?

    m_writer.writeStartDocument();
    m_writer.writeStartElement(QStringLiteral("scxml"));
    m_writer.writeDefaultNamespace(QStringLiteral("http://www.w3.org/2005/07/scxml"));
    m_writer.writeAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    if (!writeStateInner(machine))
        return false;
    m_writer.writeEndElement();
    m_writer.writeEndDocument();
    return !m_writer.hasError();
}

bool ScxmlExporter::Private::writeState(State *state)
{
    if (qobject_cast<PseudoState *>(state)) {
        return true; // pseudo states are ignored
    }

    m_writer.writeStartElement(QStringLiteral("state"));
    if (!writeStateInner(state))
        return false;
    m_writer.writeEndElement();
    return true;
}

bool ScxmlExporter::Private::writeStateInner(State *state)
{
    if (state->label().isEmpty()) {
        q->setErrorString(QStringLiteral("Encountered empty label for state: %1").arg(ObjectHelper::displayString(state)));
        return false;
    }

    if (qobject_cast<StateMachine *>(state)) {
        m_writer.writeAttribute(QStringLiteral("name"), state->label());
    } else {
        m_writer.writeAttribute(QStringLiteral("id"), state->label());
    }

    if (State *initial = ElementUtil::findInitialState(state)) {
        if (initial->label().isEmpty()) {
            q->setErrorString(QStringLiteral("Encountered empty label for state: %1").arg(ObjectHelper::displayString(initial)));
            return false;
        }
        m_writer.writeAttribute(QStringLiteral("initial"), initial->label());
    }

    const auto stateTransitions = state->transitions();
    for (const Transition *transition : stateTransitions) {
        writeTransition(transition);
    }

    const auto childStates = state->childStates();
    return std::all_of(childStates.begin(), childStates.end(),
                       [this](State *child) { return writeState(child); });
}

bool ScxmlExporter::Private::writeTransition(const Transition *transition)
{
    m_writer.writeStartElement(QStringLiteral("transition"));
    m_writer.writeAttribute(QStringLiteral("event"), transition->label());
    if (const State *targetState = transition->targetState()) {
        m_writer.writeAttribute(QStringLiteral("target"), targetState->label());
    }
    m_writer.writeEndElement();
    return true;
}
