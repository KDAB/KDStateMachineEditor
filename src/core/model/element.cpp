/*
  element.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
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

#include "element.h"
#include "elementutil.h"

#include "objecthelper.h"

#include <QDebug>
#include <QEvent>
#include <QQmlEngine>

using namespace KDSME;

namespace {

QString kindToString(PseudoState::Kind kind)
{
    switch (kind) {
    case PseudoState::InitialState:
        return QObject::tr("Initial");
    }
    return QString();
}

StateMachine* findStateMachine(const Element* element)
{
    if (!element) {
        return nullptr;
    }

    QObject *current = element->parent();
    while (current != 0) {
        if (StateMachine *machine = qobject_cast<StateMachine*>(current))
            return machine;
        current = current->parent();
    }
    return nullptr;
}

}

struct Element::Private
{
    Private()
        : m_id(0)
    {}

    QString m_label;
    quintptr m_id;
};

Element::Element(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

Element::~Element()
{
}

Element::Type Element::type() const
{
    return ElementType;
}

Element::Flags Element::flags() const
{
    return Flags(ElementIsEnabled | ElementIsSelectable | ElementIsEditable);
}

QString Element::label() const
{
    return d->m_label;
}

void Element::setLabel(const QString& label)
{
    if (d->m_label == label)
        return;

    d->m_label = label;
    emit labelChanged(label);
}

quintptr Element::internalId() const
{
    return d->m_id;
}

void Element::setInternalId(quintptr id)
{
    d->m_id = id;
}

void* Element::internalPointer() const
{
    return reinterpret_cast<void*>(d->m_id);
}

void Element::setInternalPointer(void* ptr)
{
    d->m_id = reinterpret_cast<quintptr>(ptr);
}

Element* Element::parentElement() const
{
    return qobject_cast<Element*>(parent());
}

QList<Element*> Element::childElements() const
{
    return ObjectHelper::copy_if_type<Element*>(children());
}

QString Element::toDisplayString() const
{
    const QString str = ObjectHelper::className(this, ObjectHelper::StripNameSpace);
    if (label().isEmpty()) {
        return str;
    }
    return QString("%1 [Label: %2]").arg(str).arg(label());
}

const char* Element::typeToString(Element::Type type)
{
    return ObjectHelper::enumToString(&staticMetaObject, "Type", type);
}

Element::Type Element::stringToType(const char* type)
{
    const int value = ObjectHelper::stringToEnum(&staticMetaObject, "Type", type);
    return static_cast<Element::Type>(value);
}

struct Transition::Private
{
    Private()
        : m_targetState(nullptr)
    {}

    State* m_targetState;
    QString m_guard;
};

Transition::Transition(State* sourceState)
    : Element(sourceState)
    , d(new Private)
{
}

Transition::~Transition()
{
}

Element::Type Transition::type() const
{
    return TransitionType;
}

StateMachine* Transition::machine() const
{
    StateMachine *m = findStateMachine(this);
    QQmlEngine::setObjectOwnership(m, QQmlEngine::CppOwnership);
    return m;
}

State* Transition::sourceState() const
{
    return qobject_cast<State*>(parent());
}

void Transition::setSourceState(State* sourceState)
{
    setParent(sourceState);
}

State* Transition::targetState() const
{
    return d->m_targetState;
}

void Transition::setTargetState(State* targetState)
{
    if (d->m_targetState == targetState)
        return;

    d->m_targetState = targetState;
    emit targetStateChanged(targetState);
}

QString Transition::guard() const
{
    return d->m_guard;
}

void Transition::setGuard(const QString& guard)
{
    if (d->m_guard == guard)
        return;

    d->m_guard = guard;
    emit guardChanged(d->m_guard);
}

struct SignalTransition::Private
{
    QString m_signal;
};

SignalTransition::SignalTransition(State* sourceState)
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

void SignalTransition::setSignal(const QString& signal)
{
    if (d->m_signal == signal)
        return;

    d->m_signal = signal;
    emit signalChanged(d->m_signal);
}

struct TimeoutTransition::Private
{
    Private()
        : m_timeout(0)
    {}

    int m_timeout;
};

TimeoutTransition::TimeoutTransition(State* sourceState)
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
    emit timeoutChanged(d->m_timeout);
}

struct State::Private
{
    Private()
        : m_childMode(ExclusiveStates)
        , m_isComposite(false)
    {}

    QString m_onEntry;
    QString m_onExit;
    ChildMode m_childMode;
    bool m_isComposite;
};

State::State(State* parent)
    : Element(parent)
    , d(new Private)
{
}

State::~State()
{
}

Element::Type State::type() const
{
    return StateType;
}

State* State::parentState() const
{
    return qobject_cast<State*>(parent());
}

State* State::initialState() const
{
    return ElementUtil::findInitialState(this);
}

void State::setInitialState(State* initialState)
{
    ElementUtil::setInitialState(this, initialState);
}

QList<State*> State::childStates() const
{
    return ObjectHelper::copy_if_type<State*>(children());
}

QList<Transition*> State::transitions() const
{
    return ObjectHelper::copy_if_type<Transition*>(children());
}

void State::addTransition(Transition* transition)
{
    transition->setParent(this);
}

Transition* State::addTransition(State* target)
{
    if (!target) {
        return nullptr;
    }

    Transition* transition = new Transition(this);
    transition->setTargetState(target);
    addTransition(transition);
    return transition;
}

QString State::onEntry() const
{
    return d->m_onEntry;
}

QString State::onExit() const
{
    return d->m_onExit;
}

void State::setOnEntry(const QString& onEntry)
{
    if (d->m_onEntry == onEntry)
        return;

    d->m_onEntry = onEntry;
    emit onEntryChanged(d->m_onEntry);
}

void State::setOnExit(const QString& onExit)
{
    if (d->m_onExit == onExit)
        return;

    d->m_onExit = onExit;
    emit onExitChanged(d->m_onExit);
}

State::ChildMode State::childMode() const
{
    return d->m_childMode;
}

void State::setChildMode(ChildMode childMode)
{
    if (d->m_childMode == childMode)
        return;

    d->m_childMode = childMode;
    emit childModeChanged(d->m_childMode);
}

bool State::isComposite() const
{
    return d->m_isComposite;
}

StateMachine* State::machine() const
{
    StateMachine* m = findStateMachine(this);
    QQmlEngine::setObjectOwnership(m, QQmlEngine::CppOwnership);
    return m;
}

bool State::event(QEvent* event)
{
    if (event->type() == QEvent::ChildAdded || event->type() == QEvent::ChildRemoved) {
        const bool newIsComposite = childStates().size() > 0;
        if (d->m_isComposite != newIsComposite) {
            d->m_isComposite = newIsComposite;
            emit isCompositeChanged(d->m_isComposite);
        }
    }

    return QObject::event(event);
}

struct StateMachine::Private
{
};

StateMachine::StateMachine(QObject* parent)
    : State(nullptr)
    , d(new Private)
{
    // Can't pass the parent to the State constructor, as it expects a State
    // But this works as expected regardless of whether parent is a State or not
    setParent(parent);
}

struct HistoryState::Private
{
    Private()
        : m_defaultState(nullptr)
        , m_historyType(ShallowHistory)
    {}

    State *m_defaultState;
    HistoryType m_historyType;
};

HistoryState::HistoryState(State* parent)
    : State(parent)
    , d(new Private)
{
}

HistoryState::~HistoryState()
{
}

Element::Type HistoryState::type() const
{
    return HistoryStateType;
}

QString HistoryState::toDisplayString() const
{
    const QString thisClassName = ObjectHelper::className(this, ObjectHelper::StripNameSpace);
    const QString defaultClassName = d->m_defaultState ? ObjectHelper::className(d->m_defaultState, ObjectHelper::StripNameSpace) : "None";
    return QString("%1 [Default: %2]").arg(thisClassName).arg(defaultClassName);
}

State* HistoryState::defaultState() const
{
    return d->m_defaultState;
}

void HistoryState::setDefaultState(State *state)
{
    if (d->m_defaultState == state)
        return;
    d->m_defaultState = state;
    emit defaultStateChanged(d->m_defaultState);
}

HistoryState::HistoryType HistoryState::historyType() const
{
    return d->m_historyType;
}

void HistoryState::setHistoryType(HistoryState::HistoryType historyType)
{
    if (d->m_historyType == historyType)
        return;
    d->m_historyType = historyType;
    emit historyTypeChanged();
}

struct FinalState::Private
{
};

FinalState::FinalState(State* parent)
    : State(parent)
    , d(new Private)
{
}

FinalState::~FinalState()
{
}

Element::Type FinalState::type() const
{
    return FinalStateType;
}

StateMachine::~StateMachine()
{
}

Element::Type StateMachine::type() const
{
    return StateMachineType;
}

struct PseudoState::Private
{
    Private()
        : m_kind(PseudoState::InitialState)
    {}

    Kind m_kind;
};

PseudoState::PseudoState(Kind kind, State* parent)
    : State(parent)
    , d(new Private)
{
    d->m_kind = kind;
}

PseudoState::~PseudoState()
{
}

Element::Type PseudoState::type() const
{
    return PseudoStateType;
}

Element::Flags PseudoState::flags() const
{
    auto flags = Element::flags();
    return flags & ~ElementIsEnabled;
}

PseudoState::Kind PseudoState::kind() const
{
    return d->m_kind;
}

void PseudoState::setKind(PseudoState::Kind kind)
{
    if (d->m_kind == kind)
        return;

    d->m_kind = kind;
    emit kindChanged(d->m_kind);
}

QString PseudoState::kindString() const
{
    return kindToString(d->m_kind);
}

QString PseudoState::toDisplayString() const
{
    const QString str = ObjectHelper::className(this, ObjectHelper::StripNameSpace);
    return QString("%1 [Kind: %2]").arg(str).arg(kindString());
}

QDebug KDSME::operator<<(QDebug dbg, const Transition* transition)
{
    if (!transition) {
        return dbg << static_cast<QObject*>(0);
    }
    dbg.nospace() << "Transition["
        << "this=" << (const void*)transition
        << ", source=" << transition->sourceState()
        << ", target=" << transition->targetState()
        << "]";
    return dbg.space();
}

QDebug KDSME::operator<<(QDebug dbg, const State* state)
{
    if (!state) {
        return operator<<(dbg, static_cast<QObject*>(0));
    }
    dbg.nospace() << "State["
        << "this=" << (const void*)state
        << ", label=" << state->label()
        << "]";
    return dbg.space();
}

QDebug KDSME::operator<<(QDebug dbg, const PseudoState* state)
{
    if (!state) {
        return operator<<(dbg, static_cast<QObject*>(0));
    }
    dbg.nospace() << "PseudoState["
        << "this=" << (const void*)state
        << ", kind=" << state->kindString()
        << "]";
    return dbg.space();
}

#include "moc_element.cpp"
