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

#include "elementutil.h"

#include "element.h"
#include "state.h"
#include "transition.h"

using namespace KDSME;

State *ElementUtil::findInitialState(const KDSME::State *state)
{
    if (!state)
        return nullptr;

    const auto childStates = state->childStates();
    for (State *child : childStates) {
        if (auto *pseudoState = qobject_cast<PseudoState *>(child)) {
            if (pseudoState->kind() == PseudoState::InitialState) {
                const Transition *transition = pseudoState->transitions().value(0);
                return transition ? transition->targetState() : nullptr;
            }
        }
    }
    return nullptr;
}

void ElementUtil::setInitialState(State *state, State *initialState)
{
    if (!state)
        return;

    QString pseudoStateName;
    QString transitionName;
    const auto childStates = state->childStates();
    for (State *child : childStates) {
        if (auto *pseudoState = qobject_cast<PseudoState *>(child)) {
            if (pseudoState->kind() == PseudoState::InitialState) {
                pseudoStateName = pseudoState->label();
                const Transition *transition = pseudoState->transitions().value(0);
                if (transition)
                    transitionName = transition->label();
                delete pseudoState;
            }
        }
    }

    if (!initialState)
        return;

    if (pseudoStateName.isEmpty()) {
        pseudoStateName = QStringLiteral("initalState_%1_%2").arg(state->label()).arg(initialState->label()); // clazy:exclude=qstring-arg
    }

    if (transitionName.isEmpty()) {
        transitionName = QStringLiteral("transitionInitalState_%1_%2").arg(state->label()).arg(initialState->label()); // clazy:exclude=qstring-arg
    }

    State *ps = new PseudoState(PseudoState::InitialState, state);
    ps->setLabel(pseudoStateName);

    auto *tr = new Transition(ps);
    tr->setLabel(transitionName);
    tr->setTargetState(initialState);
}

State *ElementUtil::findState(State *root, const QString &label)
{
    if (!root || root->label().isEmpty())
        return nullptr;

    if (label == root->label())
        return root;

    const auto childStates = root->childStates();
    for (State *state : childStates) {
        if (State *st = findState(state, label)) { // cppcheck-suppress useStlAlgorithm
            return st;
        }
    }

    return nullptr;
}

StateMachine *ElementUtil::findStateMachine(const Element *element)
{
    QObject *current = const_cast<Element *>(element);
    while (current != nullptr) {
        if (auto *machine = qobject_cast<StateMachine *>(current))
            return machine;
        current = current->parent();
    }
    return nullptr;
}
