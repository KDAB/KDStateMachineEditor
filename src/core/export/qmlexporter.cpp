/*
  qmlexporter.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Sebastian Sauer <sebastian.sauer@kdab.com>

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

#include "export/qmlexporter.h"

#include "objecthelper.h"
#include "element.h"
#include "elementutil.h"

#include <QDebug>
#include <QTextStream>

#include <algorithm>

using namespace KDSME;

static const char KDSME_QML_MODULE[] = "QtQml.StateMachine";
static const char KDSME_QML_MODULE_VERSION[] = "1.0";

namespace {

class LevelIncrementer
{
public:
    LevelIncrementer(int *level) : m_level(level) { ++(*m_level); }
    ~LevelIncrementer() { --(*m_level); }
private:
    int *m_level;
};

/// Maps the type of @p element to the QML Component ID
QString elementToComponent(Element* element)
{
    const QString customType = element->property("com.kdab.KDSME.DSMExporter.customType").toString();
    if (!customType.isEmpty())
        return customType;

    const Element::Type type = element->type();
    switch (type) {
    case Element::StateMachineType:
        return "StateMachine";
    case Element::FinalStateType:
        return "FinalState";
    case Element::HistoryStateType:
        return "HistoryState";
    case Element::StateType:
        return "StateBase";
    case Element::TransitionType:
        return "Transition";
    case Element::SignalTransitionType:
        return "SignalTransition";
    case Element::TimeoutTransitionType:
        return "TimeoutTransition";
    default:
        Q_ASSERT(false);
        return QString();
    }
}

/// Turn input @p input into a usable QML ID (remove invalid chars)
QString toQmlId(const QString& input)
{
    if (input.isEmpty())
        return input;

    QString out = input;
    std::replace_if(out.begin(), out.end(),
        [](const QChar& c) -> bool {
            return !(c.isLetterOrNumber() || c == '_');
        }, QChar('_')
    );

    out[0] = out.at(0).toLower();
    return out;
}

}

struct QmlExporter::Private
{
    Private(QByteArray* array);
    Private(QIODevice* device);

    bool writeStateMachine(StateMachine* machine);
    bool writeState(State* state);
    bool writeStateInner(State* state);
    bool writeTransition(Transition* transition);

    QString indention() const;

    QTextStream m_out;
    int m_indent, m_level;
};

QmlExporter::Private::Private(QByteArray* array)
    : m_out(array)
    , m_indent(4)
    , m_level(0)
{
}

QmlExporter::Private::Private(QIODevice* device)
    : m_out(device)
    , m_indent(4)
    , m_level(0)
{
}

QmlExporter::QmlExporter(QByteArray* array)
    : d(new Private(array))
{
    Q_ASSERT(array);
}

QmlExporter::QmlExporter(QIODevice* device)
    : d(new Private(device))
{
    Q_ASSERT(device);
}

QmlExporter::~QmlExporter()
{
}

int QmlExporter::indent() const
{
    return d->m_indent;
}

void QmlExporter::setIndent(int indent)
{
    d->m_indent = indent;
}

bool QmlExporter::exportMachine(StateMachine* machine)
{
    setErrorString(QString());
    d->m_level = 0;

    if (!machine) {
        setErrorString("Null machine instance passed");
        return false;
    }

    if (d->m_out.status() != QTextStream::Ok) {
        setErrorString(QString("Invalid QTextStream status: %1").arg(d->m_out.status()));
        return false;
    }

    bool success = d->writeStateMachine(machine);
    d->m_out.flush();
    return success;
}

bool QmlExporter::Private::writeStateMachine(StateMachine* machine)
{
    Q_ASSERT(machine);

    const QString importStmt = QString("import %1 %2\n").arg(KDSME_QML_MODULE).arg(KDSME_QML_MODULE_VERSION);
    m_out << indention() << importStmt;

    const QStringList customImports = machine->property("com.kdab.KDSME.DSMExporter.customImports").toStringList();
    foreach (const QString &customImport, customImports)
        m_out << customImport << '\n';
    m_out << '\n';

    const QString qmlComponent = elementToComponent(machine);
    m_out << indention() << QString("%1 {\n").arg(qmlComponent);
    if (!writeStateInner(machine))
        return false;
    m_out << indention() << QString("}\n");
    return true;
}

bool QmlExporter::Private::writeState(State* state)
{
    Q_ASSERT(state);

    if (qobject_cast<PseudoState*>(state))
        return true; // pseudo states are ignored

    const QString qmlComponent = elementToComponent(state);
    m_out << indention() << QString("%1 {\n").arg(qmlComponent);
    if (!writeStateInner(state))
        return false;
    m_out << indention() << QString("}\n");
    return true;
}

bool QmlExporter::Private::writeStateInner(State* state)
{
    Q_ASSERT(state);

    LevelIncrementer levelinc(&m_level);
    Q_UNUSED(levelinc);

    if (!state->label().isEmpty()) {
        m_out << indention() << QString("id: %1\n").arg(toQmlId(state->label()));
    }

    if (StateMachine *stateMachine = qobject_cast<StateMachine*>(state)) {
        const QString running = stateMachine->property("com.kdab.KDSME.DSMExporter.running").toString();
        if (!running.isEmpty())
            m_out << indention() << QString("running: %1\n").arg(running);
    }

    if (state->childMode() == State::ParallelStates) {
        m_out << indention() << "childMode: QState.ParallelStates\n";
    }

    if (State* initial = ElementUtil::findInitialState(state)) {
        m_out << indention() << QString("initialState: %1\n").arg(initial->label());
    }

    if (HistoryState *historyState = qobject_cast<HistoryState*>(state)) {
        if (State *defaultState = historyState->defaultState())
            m_out << indention() << QString("defaultState: %1\n").arg(toQmlId(defaultState->label()));
        if (historyState->historyType() == HistoryState::DeepHistory)
            m_out << indention() << "historyType: HistoryState.DeepHistory\n";
    }

    if (!state->onEntry().isEmpty()) {
        m_out << indention() << "onEntered: " << state->onEntry() << '\n';
    }
    if (!state->onExit().isEmpty()) {
        m_out << indention() << "onExited: " << state->onExit() << '\n';
    }

    foreach (State* child, state->childStates()) {
        if (!writeState(child))
            return false;
    }

    foreach (Transition* transition, state->transitions()) {
        if (!writeTransition(transition))
            return false;
    }

    return true;
}

bool QmlExporter::Private::writeTransition(Transition* transition)
{
    Q_ASSERT(transition);
    m_out << indention() << QString("%1 {\n").arg(elementToComponent(transition));
    {
        LevelIncrementer levelinc(&m_level);
        Q_UNUSED(levelinc);

        if (!transition->label().isEmpty()) {
            m_out << indention() << QString("id: %1\n").arg(toQmlId(transition->label()));
        }
        if (transition->targetState()) {
            m_out << indention() << QString("targetState: %1\n").arg(transition->targetState()->label());
        }
        if (transition->type() == Element::SignalTransitionType) {
            auto t = qobject_cast<SignalTransition*>(transition);
            if (!t->signal().isEmpty()) {
                m_out << indention() << QString("signal: %1").arg(t->signal()) << '\n';
            }
        }
        if (transition->type() == Element::TimeoutTransitionType) {
            auto t = qobject_cast<TimeoutTransition*>(transition);
            if (t->timeout() != -1) {
                m_out << indention() << QString("timeout: %1").arg(t->timeout()) << '\n';
            }
        }
        if (!transition->guard().isEmpty()) {
            m_out << indention() << "guard: " << transition->guard() << '\n';
        }
    }
    m_out << indention() << QString("}\n");
    return true;
}

QString QmlExporter::Private::indention() const
{
    return QString().fill(QChar(' '), m_indent*m_level);
}
