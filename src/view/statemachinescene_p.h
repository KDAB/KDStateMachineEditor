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

#ifndef KDSME_VIEW_STATEMACHINESCENE_P_H
#define KDSME_VIEW_STATEMACHINESCENE_P_H

#include "statemachinescene.h"

class QuickSceneItem;

namespace KDSME {

struct StateMachineScene::Private
{
    explicit Private(StateMachineScene *view);

    State *importState(State *state);

    void zoomByInternal(qreal scale) const;

    void updateItemVisibilities() const;
    void updateChildItemVisibility(State *state, bool expand);

    void setRootElement(State *root);

    void importTransitions(State *state);
    Transition *importTransition(Transition *transition);

    StateMachineScene *q;

    State *m_rootState;
    Layouter *m_layouter;
    LayoutProperties *m_properties;
    qreal m_zoom;
    int m_maximumDepth;
};

}

#endif
