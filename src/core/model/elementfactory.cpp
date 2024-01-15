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

#include "elementfactory.h"

#include "state.h"
#include "transition.h"

using namespace KDSME;

ElementFactory::ElementFactory()
{
}

Element *ElementFactory::create(Element::Type type) const
{
    switch (type) {
    case Element::SignalTransitionType:
        return new SignalTransition(nullptr);
    case Element::TimeoutTransitionType:
        return new TimeoutTransition(nullptr);
    case Element::StateType:
        return new State;
    case Element::HistoryStateType:
        return new HistoryState;
    case Element::StateMachineType:
        return new StateMachine;
    case Element::FinalStateType:
        return new FinalState;
    case Element::PseudoStateType:
        return new PseudoState;
    default:
        return nullptr;
    }
}
