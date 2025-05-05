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

#include "export/qmlexporter.h"

#include "objecthelper.h"
#include "element.h"
#include "elementutil.h"
#include "state.h"
#include "transition.h"
#include "debug.h"

#include <QTextStream>

#include <algorithm>

using namespace KDSME;

namespace {

#define KDSME_QML_MODULE "QtQml.StateMachine"
#define KDSME_QML_MODULE_VERSION "1.0"

class LevelIncrementer // clazy:exclude=rule-of-three
{
public:
    explicit LevelIncrementer(int *level)
        : m_level(level)
    {
        ++(*m_level);
    }
    ~LevelIncrementer()
    {
        --(*m_level);
    }

private:
    int *m_level;
};

/// Maps the type of @p element to the QML Component ID
QString elementToComponent(Element *element)
{
    QString customType = element->property("com.kdab.KDSME.DSMExporter.customType").toString();
    if (!customType.isEmpty())
        return customType;

    const Element::Type type = element->type();
    switch (type) {
    case Element::StateMachineType:
        return QStringLiteral("StateMachine");
    case Element::FinalStateType:
        return QStringLiteral("FinalState");
    case Element::HistoryStateType:
        return QStringLiteral("HistoryState");
    case Element::StateType:
        return QStringLiteral("State");
    case Element::TransitionType: // fall-through
    case Element::SignalTransitionType:
        return QStringLiteral("SignalTransition");
    case Element::TimeoutTransitionType:
        return QStringLiteral("TimeoutTransition");
    case Element::PseudoStateType: // fall-through
    case Element::ElementType:
        return QString();
    }

    Q_UNREACHABLE();
    return QString();
}

/// Turn input @p input into a usable QML ID (remove invalid chars)
QString toQmlId(const QString &input)
{
    if (input.isEmpty())
        return input;

    QString out = input;
    std::replace_if(
        out.begin(), out.end(),
        [](QChar c) -> bool {
            return !(c.isLetterOrNumber() || c == u'_');
        },
        u'_');

    out[0] = out.at(0).toLower();
    return out;
}

}

struct QmlExporter::Private
{
    explicit Private(QByteArray *array);
    explicit Private(QIODevice *device);

    bool writeStateMachine(StateMachine *machine);
    bool writeState(State *state);
    bool writeStateInner(State *state);
    bool writeTransition(Transition *transition);
    void writeAttribute(Element *element, const QString &name, const QString &value);

    [[nodiscard]] QString indention() const;

    QTextStream m_out;
    int m_indent, m_level;
};

QmlExporter::Private::Private(QByteArray *array)
    : m_out(array)
    , m_indent(4)
    , m_level(0)
{
}

QmlExporter::Private::Private(QIODevice *device)
    : m_out(device)
    , m_indent(4)
    , m_level(0)
{
}

QmlExporter::QmlExporter(QByteArray *array)
    : d(new Private(array))
{
    Q_ASSERT(array);
}

QmlExporter::QmlExporter(QIODevice *device)
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

bool QmlExporter::exportMachine(StateMachine *machine)
{
    setErrorString(QString());
    d->m_level = 0;

    if (!machine) {
        setErrorString(QStringLiteral("Null machine instance passed"));
        return false;
    }

    if (d->m_out.status() != QTextStream::Ok) {
        setErrorString(QStringLiteral("Invalid QTextStream status: %1").arg(d->m_out.status()));
        return false;
    }

    const bool success = d->writeStateMachine(machine);
    d->m_out.flush();
    return success;
}

bool QmlExporter::Private::writeStateMachine(StateMachine *machine)
{
    Q_ASSERT(machine);

    const QString importStmt = QStringLiteral("import %1 %2\n").arg(QStringLiteral(KDSME_QML_MODULE)).arg(QStringLiteral(KDSME_QML_MODULE_VERSION)); // clazy:exclude=qstring-arg
    m_out << indention() << importStmt;

    const QStringList customImports = machine->property("com.kdab.KDSME.DSMExporter.customImports").toStringList();
    for (const QString &customImport : customImports)
        m_out << customImport << '\n';
    m_out << '\n';

    const QString qmlComponent = elementToComponent(machine);
    m_out << indention() << QStringLiteral("%1 {\n").arg(qmlComponent);
    if (!writeStateInner(machine))
        return false;
    m_out << indention() << QStringLiteral("}\n");
    return true;
}

bool QmlExporter::Private::writeState(State *state)
{
    Q_ASSERT(state);

    if (qobject_cast<PseudoState *>(state))
        return true; // pseudo states are ignored

    if (!state->property("com.kdab.KDSME.DSMExporter.externalSource").isNull())
        return true; // external defined states, like sourced from other qml files, are not exported

    const QString qmlComponent = elementToComponent(state);
    m_out << indention() << QStringLiteral("%1 {\n").arg(qmlComponent);
    if (!writeStateInner(state))
        return false;
    m_out << indention() << QStringLiteral("}\n");
    return true;
}

void QmlExporter::Private::writeAttribute(Element *element, const QString &name, const QString &value)
{
    if (value.isEmpty())
        return;

    const QVariant implBindingNames = element->property("com.kdab.KDSME.DSMExporter.implBindingNames");
    if (!implBindingNames.isNull()) {
        const QString v = implBindingNames.toMap().value(name).toString();
        if (value == v) {
            return;
        }
    }

    m_out << indention() << QStringLiteral("%1: %2\n").arg(name).arg(value); // clazy:exclude=qstring-arg
}

bool QmlExporter::Private::writeStateInner(State *state)
{
    Q_ASSERT(state);

    const LevelIncrementer levelinc(&m_level);
    Q_UNUSED(levelinc);

    writeAttribute(state, QStringLiteral("id"), toQmlId(state->label()));

    if (auto *stateMachine = qobject_cast<StateMachine *>(state)) {
        const QString running = stateMachine->property("com.kdab.KDSME.DSMExporter.running").toString();
        writeAttribute(state, QStringLiteral("running"), running);
    }

    if (state->childMode() == State::ParallelStates) {
        writeAttribute(state, QStringLiteral("childMode"), QStringLiteral("State.ParallelStates"));
    }

    if (const State *initial = ElementUtil::findInitialState(state)) {
        writeAttribute(state, QStringLiteral("initialState"), toQmlId(initial->label()));
    }

    if (auto *historyState = qobject_cast<HistoryState *>(state)) {
        if (const State *defaultState = historyState->defaultState())
            writeAttribute(state, QStringLiteral("defaultState"), toQmlId(defaultState->label()));
        if (historyState->historyType() == HistoryState::DeepHistory)
            writeAttribute(state, QStringLiteral("historyType"), QStringLiteral("HistoryState.DeepHistory"));
    }

    writeAttribute(state, QStringLiteral("onEntered"), state->onEntry());
    writeAttribute(state, QStringLiteral("onExited"), state->onExit());

    const auto childStates = state->childStates();
    if (std::any_of(childStates.begin(), childStates.end(),
                    [this](State *child) { return !writeState(child); })) {
        return false;
    }

    const auto stateTransitions = state->transitions();
    return std::all_of(stateTransitions.begin(), stateTransitions.end(),
                       [this](Transition *child) { return writeTransition(child); });
}

bool QmlExporter::Private::writeTransition(Transition *transition)
{
    Q_ASSERT(transition);
    m_out << indention() << QStringLiteral("%1 {\n").arg(elementToComponent(transition));
    {
        const LevelIncrementer levelinc(&m_level);
        Q_UNUSED(levelinc);

        writeAttribute(transition, QStringLiteral("id"), toQmlId(transition->label()));

        if (transition->targetState()) {
            writeAttribute(transition, QStringLiteral("targetState"), toQmlId(transition->targetState()->label()));
        }

        if (transition->type() == Element::SignalTransitionType) {
            auto t = qobject_cast<SignalTransition *>(transition);
            writeAttribute(transition, QStringLiteral("signal"), t->signal());
        }

        if (transition->type() == Element::TimeoutTransitionType) {
            auto t = qobject_cast<TimeoutTransition *>(transition);
            if (t->timeout() != -1) {
                writeAttribute(transition, QStringLiteral("timeout"), QString::number(t->timeout()));
            }
        }

        writeAttribute(transition, QStringLiteral("guard"), transition->guard());
    }
    m_out << indention() << QStringLiteral("}\n");
    return true;
}

QString QmlExporter::Private::indention() const
{
    return QString { m_indent * m_level, u' ' };
}
