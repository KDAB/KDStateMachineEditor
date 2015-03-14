/*
  elementutil.cpp

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

#include "elementutil.h"

#include "element.h"
#include "state.h"
#include "transition.h"

using namespace KDSME;

State* ElementUtil::findInitialState(const KDSME::State* state)
{
    if (!state)
        return nullptr;

    foreach (State* child, state->childStates()) {
        if (PseudoState* pseudoState = qobject_cast<PseudoState*>(child)) {
            if (pseudoState->kind() == PseudoState::InitialState) {
                Transition* transition = pseudoState->transitions().value(0);
                return transition ? transition->targetState() : nullptr;
            }
        }
    }
    return nullptr;
}

void ElementUtil::setInitialState(State* state, State* initialState)
{
    if (!state)
        return;

    QString pseudoStateName, transitionName;
    foreach (State* child, state->childStates()) {
        if (PseudoState* pseudoState = qobject_cast<PseudoState*>(child)) {
            if (pseudoState->kind() == PseudoState::InitialState) {
                pseudoStateName = pseudoState->label();
                Transition* transition = pseudoState->transitions().value(0);
                if (transition)
                    transitionName = transition->label();
                delete pseudoState;
            }
        }
    }

    if (!initialState)
        return;

    if (pseudoStateName.isEmpty())
        pseudoStateName = QString("initalState_%1_%2").arg(state->label()).arg(initialState->label());

    if (transitionName.isEmpty())
        transitionName = QString("transitionInitalState_%1_%2").arg(state->label()).arg(initialState->label());

    State* ps = new PseudoState(PseudoState::InitialState, state);
    ps->setLabel(pseudoStateName);

    Transition* tr = new Transition(ps);
    tr->setLabel(transitionName);
    tr->setTargetState(initialState);
}

State* ElementUtil::findState(State* root, const QString &label)
{
    if (!root || root->label().isEmpty())
        return nullptr;

    if (label == root->label())
        return root;

    foreach (State* state, root->childStates())
        if (State* st = findState(state, label))
            return st;

    return nullptr;
}

StateMachine* ElementUtil::findStateMachine(const Element* element)
{
    QObject* current = const_cast<Element*>(element);
    while (current != nullptr) {
        if (StateMachine *machine = qobject_cast<StateMachine*>(current))
            return machine;
        current = current->parent();
    }
    return nullptr;
}
