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

#include "graphvizlayerlayouter.h"

#include "graphvizlayouterbackend_p.h"
#include "element.h"
#include "state.h"
#include "transition.h"

#include "debug.h"

#include <QRectF>

using namespace KDSME;

GraphvizLayerLayouter::GraphvizLayerLayouter(QObject *parent)
    : Layouter(parent)
    , m_backend(new GraphvizLayouterBackend)
{
    m_backend->setLayoutMode(GraphvizLayouterBackend::NonRecursiveMode);
}

GraphvizLayerLayouter::~GraphvizLayerLayouter()
{
    delete m_backend;
}

QRectF GraphvizLayerLayouter::layout(State *state, const LayoutProperties *properties)
{
    m_backend->openLayout(state, properties);

    const QList<State *> childStates = state->childStates();

    // Step 1: Create Graphviz structures out of the State/Transition tree
    // Step 1.1: build nodes
    for (State *childState : childStates) {
        m_backend->buildState(childState);
    }
    // Step 1.2: build edges
    for (const State *childState : childStates) {
        const auto stateTransitions = childState->transitions();
        for (Transition *transition : stateTransitions) {
            // TODO: What to do with transitions crossing hierarchies?
            if (!childStates.contains(transition->targetState())) {
                continue; // ignore for now
            }
            m_backend->buildTransition(transition);
        }
    }

    m_backend->layout();

    // Step 2: Import the information from the Graphviz structures to the State/Transition tree
    m_backend->import();

    const QRectF boundingRect = m_backend->boundingRect();
    m_backend->closeLayout();
    return boundingRect;
}
