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
// krazy:excludeall=captruefalse as TRUE/FALSE are defined by graphviz

#include "gvutils.h"

#include <graphviz/gvc.h>
#ifdef WITH_CGRAPH
#include <graphviz/cgraph.h>
#else
#include <graphviz/graph.h>
#endif

using namespace KDSME;

constexpr int MOVE_TO_FRONT = 1;

#ifdef WITH_CGRAPH
Agraph_t *GVUtils::_agopen(const QString &name, Agdesc_t kind, Agdisc_t *disc) // cppcheck-suppress passedByValue
#else
Agraph_t *GVUtils::_agopen(const QString &name, int kind)
#endif
{
#ifdef WITH_CGRAPH
    return agopen(name.toLocal8Bit().data(), kind, disc);
#else
    return agopen(name.toLocal8Bit().data(), kind);
#endif
}

QString GVUtils::_agget(void *object, const QString &attr, const QString &alt)
{
    QString str = QString::fromUtf8(agget(object, attr.toUtf8().data()));
    if (str.isEmpty()) {
        return alt;
    }
    return str;
}

Agsym_t *GVUtils::_agnodeattr(Agraph_t *object, const QString &attr, const QString &alt)
{
#ifdef WITH_CGRAPH
    return agattr(object, AGNODE,
#else
    return agnodeattr(object,
#endif
                  attr.toLocal8Bit().data(),
                  alt.toLocal8Bit().data());
}

Agsym_t *GVUtils::_agedgeattr(Agraph_t *object, const QString &attr, const QString &alt)
{
#ifdef WITH_CGRAPH
    return agattr(object, AGEDGE,
#else
    return agedgeattr(object,
#endif
                  attr.toLocal8Bit().data(),
                  alt.toLocal8Bit().data());
}

#ifdef WITH_CGRAPH
void *GVUtils::_agbindrec(void *obj, const char *name, unsigned int size, int move_to_front)
{
    return agbindrec(obj, const_cast<char *>(name), size, move_to_front);
}
#endif

Agnode_t *GVUtils::_agnode(Agraph_t *graph, const QString &attr, bool create)
{
#ifdef WITH_CGRAPH
    Agnode_t *n = agnode(graph, attr.toLocal8Bit().data(), create);
    _agbindrec(n, "Agnodeinfo_t", sizeof(Agnodeinfo_t), MOVE_TO_FRONT);
    return n;
#else
    Q_UNUSED(create);
    return agnode(graph, attr)));
#endif
}

Agedge_t *GVUtils::_agedge(Agraph_t *graph, Agnode_t *tail, Agnode_t *head,
                           const QString &name, bool create)
{
#ifdef WITH_CGRAPH
    Agedge_t *e = agedge(graph, tail, head, name.toLocal8Bit().data(), create);
    _agbindrec(e, "Agedgeinfo_t", sizeof(Agedgeinfo_t), MOVE_TO_FRONT);
    return e;
#else
    Q_UNUSED(name);
    Q_UNUSED(create);
    return agedge(graph, tail, head);
#endif
}

Agraph_t *GVUtils::_agsubg(Agraph_t *graph, const QString &attr, bool create)
{
#ifdef WITH_CGRAPH
    Agraph_t *g = agsubg(graph, attr.toLocal8Bit().data(), create);
    _agbindrec(g, "Agraphinfo_t", sizeof(Agraphinfo_t), MOVE_TO_FRONT);
    return g;
#else
    Q_UNUSED(create);
    return agsubg(graph, attr.toLocal8Bit().data());
#endif
}

int GVUtils::_agset(void *object, const QString &attr, const QString &value)
{
    return agsafeset(object, attr.toLocal8Bit().data(),
                     value.toLocal8Bit().data(), const_cast<char *>(""));
}

int GVUtils::_gvLayout(GVC_t *gvc, graph_t *g, const char *engine)
{
    return gvLayout(gvc, g, engine);
}
