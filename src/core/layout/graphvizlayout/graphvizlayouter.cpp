/*
  graphvizlayouter.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "graphvizlayouter.h"

#include "graphvizlayouterbackend.h"
#include "element.h"

#include <QDebug>
#include <QRectF>

using namespace KDSME;

GraphvizLayouter::GraphvizLayouter(QObject* parent)
    : Layouter(parent)
    , m_backend(new GraphvizLayouterBackend)
{
}

GraphvizLayouter::~GraphvizLayouter()
{
    delete m_backend;
}

QRectF GraphvizLayouter::layout(StateLayoutItem* state, View* view)
{
    if (!state) {
        qWarning() << "Null state passed to layout function";
        return QRectF();
    }

    if (!view) {
        qWarning() << "Null view passed to layout function";
        return QRectF();
    }

    qDebug() << Q_FUNC_INFO << state << view;

    // open context
    //const QString id = state->label();
    m_backend->openContext();

    // Step 1: Create Graphviz structures out of the State/Transition tree
    // Step 1.1: build nodes
    m_backend->buildState(state);
    // Step 1.2: build edges
    m_backend->buildTransitions(state);

    m_backend->layout();

    // Step 2: Import the information from the Graphviz structures to the State/Transition tree
    m_backend->import();

    const QRectF boundingRect = m_backend->boundingRect();

    m_backend->closeContext();
    return boundingRect;
}
