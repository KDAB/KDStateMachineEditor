/*
  elementfactory.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "elementfactory.h"

#include "state.h"
#include "transition.h"

using namespace KDSME;

ElementFactory::ElementFactory()
{
}

Element* ElementFactory::create(Element::Type type) const
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
