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

#ifndef KDSME_LAYOUT_GVUTILS_H
#define KDSME_LAYOUT_GVUTILS_H

#include <config-kdsme.h>

#include <graphviz/types.h>

#include <QString>

namespace KDSME {

namespace GVUtils {

/// The agopen method for opening a graph
#ifdef WITH_CGRAPH
extern Agraph_t *_agopen(const QString &name, Agdesc_t kind, Agdisc_t *disc);
#else
extern Agraph_t *_agopen(const QString &name, int kind);
#endif

/// Add an alternative value parameter to the method for getting an object's attribute
QString _agget(void *object, const QString &attr, const QString &alt = QString());

Agsym_t *_agnodeattr(Agraph_t *object, const QString &attr,
                     const QString &alt = QString());
Agsym_t *_agedgeattr(Agraph_t *object, const QString &attr,
                     const QString &alt = QString());

int _gvLayout(GVC_t *gvc, graph_t *g, const char *engine);

#ifdef WITH_CGRAPH
void *_agbindrec(void *obj, const char *name, unsigned int size, int move_to_front);
#endif

Agnode_t *_agnode(Agraph_t *graph, const QString &attr, bool create = true);
Agedge_t *_agedge(Agraph_t *graph, Agnode_t *tail, Agnode_t *head,
                  const QString &name = QString(), bool create = true);
Agraph_t *_agsubg(Agraph_t *graph, const QString &attr, bool create = true);

/// Directly use agsafeset which always works, contrarily to agset
int _agset(void *object, const QString &attr, const QString &value);

}

}

#endif
