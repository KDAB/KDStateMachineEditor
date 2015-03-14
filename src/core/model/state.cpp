/*
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

#include "state.h"

#include "elementutil.h"
#include "objecthelper.h"
#include "runtimecontroller.h"
#include "transition.h"

#include <QDebug>
#include <QEvent>
#include <QQmlEngine>

using namespace KDSME;

namespace {

struct StandardRuntimeController : public RuntimeController
{
    Q_OBJECT
};

QString kindToString(PseudoState::Kind kind)
{
    switch (kind) {
    case PseudoState::InitialState:
        return QObject::tr("Initial");
    }
    return QString();
}

}

struct State::Private
{
    Private()
        : m_childMode(ExclusiveStates)
        , m_isComposite(false)
        , m_isExpanded(true)
    {}

    QString m_onEntry;
    QString m_onExit;
    ChildMode m_childMode;
    bool m_isComposite;
    bool m_isExpanded;
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

bool State::isExpanded() const
{
    return d->m_isExpanded;
}

void State::setExpanded(bool expanded)
{
    if (d->m_isExpanded == expanded)
        return;

    d->m_isExpanded = expanded;
    emit expandedChanged(d->m_isExpanded);
}

StateMachine* State::machine() const
{
    StateMachine* m = ElementUtil::findStateMachine(this);
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
    Private(StateMachine *q)
        : q(q)
        , m_runtimeController(new StandardRuntimeController)
    {
    }

    ~Private()
    {
        if (qobject_cast<StandardRuntimeController*>(m_runtimeController)) {
            delete m_runtimeController;
        }
    }

    StateMachine* q;

    RuntimeController* m_runtimeController;
};

StateMachine::StateMachine(QObject* parent)
    : State(nullptr)
    , d(new Private(this))
{
    // Can't pass the parent to the State constructor, as it expects a State
    // But this works as expected regardless of whether parent is a State or not
    setParent(parent);

    setWidth(128);
    setHeight(128);
}

RuntimeController* StateMachine::runtimeController() const
{
    return d->m_runtimeController;
}

void StateMachine::setRuntimeController(RuntimeController* runtimeController)
{
    if (d->m_runtimeController == runtimeController)
        return;

    if (qobject_cast<StandardRuntimeController*>(d->m_runtimeController)) {
        d->m_runtimeController->deleteLater();
    }
    d->m_runtimeController = runtimeController;
    if (!d->m_runtimeController) {
        d->m_runtimeController = new StandardRuntimeController;
    }

    emit runtimeControllerChanged(d->m_runtimeController);
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

HistoryState::HistoryState(HistoryState::HistoryType type, State* parent)
    : State(parent)
    , d(new Private)
{
    d->m_historyType = type;
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

QDebug KDSME::operator<<(QDebug dbg, const State* state)
{
    if (!state) {
        return operator<<(dbg, static_cast<QObject*>(nullptr));
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
        return operator<<(dbg, static_cast<QObject*>(nullptr));
    }
    dbg.nospace() << "PseudoState["
        << "this=" << (const void*)state
        << ", kind=" << state->kindString()
        << "]";
    return dbg.space();
}

#include "state.moc"
