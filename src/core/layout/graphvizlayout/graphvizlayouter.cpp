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

#include "graphvizlayouter.h"

#include "graphvizlayouterbackend_p.h"
#include "element.h"
#include "state.h"

#include "debug.h"

#include <QRectF>

using namespace KDSME;

GraphvizLayouter::GraphvizLayouter(QObject *parent)
    : Layouter(parent)
    , m_backend(new GraphvizLayouterBackend)
{
}

GraphvizLayouter::~GraphvizLayouter()
{
    delete m_backend;
}

QRectF GraphvizLayouter::layout(State *state, const LayoutProperties *properties)
{
    if (!state) {
        qCWarning(KDSME_CORE) << "Null state passed to layout function";
        return QRectF();
    }

    qCDebug(KDSME_CORE) << state << properties;

    // open context
    m_backend->openLayout(state, properties);

    // Step 1: Create Graphviz structures out of the State/Transition tree
    // Step 1.1: build nodes
    m_backend->buildState(state);
    // Step 1.2: build edges
    m_backend->buildTransitions(state);

    m_backend->layout();

    // Step 2: Import the information from the Graphviz structures to the State/Transition tree
    m_backend->import();

    const QRectF boundingRect = m_backend->boundingRect();
    m_backend->closeLayout();
    return boundingRect;
}
