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

#ifndef KDSME_LAYOUT_GRAPHVIZLAYOUTER_H
#define KDSME_LAYOUT_GRAPHVIZLAYOUTER_H

#include "kdsme_core_export.h"

#include "layouter.h"

class GraphvizLayouterBackend;

namespace KDSME {

class StateMachineScene;

class KDSME_CORE_EXPORT GraphvizLayouter : public Layouter
{
    Q_OBJECT

public:
    explicit GraphvizLayouter(QObject *parent = nullptr);
    ~GraphvizLayouter();

    QRectF layout(State *state, const LayoutProperties *properties) override;

private:
    GraphvizLayouterBackend *m_backend;
};

}

#endif // GRAPHVIZLAYOUTER_H
