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

#ifndef KDSME_MODEL_ELEMENTUTIL_H
#define KDSME_MODEL_ELEMENTUTIL_H

#include "kdsme_core_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace KDSME {

class CompositeState;
class Element;
class State;
class StateMachine;

namespace ElementUtil {

KDSME_CORE_EXPORT State *findInitialState(const State *state);
KDSME_CORE_EXPORT void setInitialState(State *state, State *initialState);

KDSME_CORE_EXPORT State *findState(State *root, const QString &label);
KDSME_CORE_EXPORT StateMachine *findStateMachine(const Element *element);

}

}

#endif
