/*
  gvutils.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
//krazy:excludeall=captruefalse as TRUE/FALSE are defined by graphviz

#include "gvutils.h"

#include <graphviz/gvc.h>
#ifdef WITH_CGRAPH
#  include <graphviz/cgraph.h>
#else
#  include <graphviz/graph.h>
#endif

using namespace KDSME;

#ifdef WITH_CGRAPH
Agraph_t *GVUtils::_agopen(const QString &name, Agdesc_t kind, Agdisc_t *disc)
#else
Agraph_t *GVUtils::_agopen(const QString &name, int kind)
#endif
{
#ifdef WITH_CGRAPH
  return agopen(const_cast<char *>(qPrintable(name)), kind, disc);
#else
  return agopen(const_cast<char *>(qPrintable(name)), kind);
#endif
}

QString GVUtils::_agget(void *object, const QString &attr, const QString &alt)
{
  const QString str = agget(object, const_cast<char *>(qPrintable(attr)));
  if(str.isEmpty()) {
    return alt;
  } else {
    return str;
  }
}

Agsym_t *GVUtils::_agnodeattr(Agraph_t *object, const QString &attr, const QString &alt)
{
#ifdef WITH_CGRAPH
  return agattr(object, AGNODE,
#else
  return agnodeattr(object,
#endif
                const_cast<char *>(qPrintable(attr)),
                const_cast<char *>(qPrintable(alt)));
}

Agsym_t *GVUtils::_agedgeattr(Agraph_t *object, const QString &attr, const QString &alt)
{
#ifdef WITH_CGRAPH
  return agattr(object, AGEDGE,
#else
  return agedgeattr(object,
#endif
                const_cast<char *>(qPrintable(attr)),
                const_cast<char *>(qPrintable(alt)));
}

Agnode_t *GVUtils::_agnode(Agraph_t *graph, const QString &attr, bool create)
{
#ifdef WITH_CGRAPH
  Agnode_t *n = agnode(graph, const_cast<char*>(qPrintable(attr)), create);
  agbindrec(n, "Agnodeinfo_t", sizeof(Agnodeinfo_t), TRUE);
  return n;
#else
  Q_UNUSED(create);
  return agnode(graph, const_cast<char*>(qPrintable(attr)));
#endif
}

Agedge_t *GVUtils::_agedge(Agraph_t *graph, Agnode_t *tail, Agnode_t *head,
                           const QString &name, bool create)
{
#ifdef WITH_CGRAPH
  Agedge_t *e = agedge(graph, tail, head, const_cast<char*>(qPrintable(name)), create);
  agbindrec(e, "Agedgeinfo_t", sizeof(Agedgeinfo_t), TRUE);
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
  Agraph_t *g = agsubg(graph, const_cast<char*>(qPrintable(attr)), create);
  agbindrec(g, "Agraphinfo_t", sizeof(Agraphinfo_t), TRUE);
  return g;
#else
  Q_UNUSED(create);
  return agsubg(graph, const_cast<char*>(qPrintable(attr)));
#endif
}

int GVUtils::_agset(void *object, const QString &attr, const QString &value)
{
  return agsafeset(object, const_cast<char *>(qPrintable(attr)),
                   const_cast<char *>(qPrintable(value)), const_cast<char *>(""));
}

int GVUtils::_gvLayout(GVC_t *gvc, graph_t *g, const char *engine)
{
  return gvLayout(gvc, g, const_cast<char*>(engine));
}
