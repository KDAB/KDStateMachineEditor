/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "depthchecker.h"

using namespace KDSME;

namespace {

int elementDepth(const Element* element)
{
    if (!element)
        return -1;

    int depth = 0;
    const Element* current = element;
    while (current) {
        ++depth;
        current = current->parentElement();
    }
    return depth;
}

}

struct DepthChecker::Private
{
    Private(DepthChecker* q)
        : q(q)
        , m_target(nullptr)
        , m_depth(-1)
    {}

    void updateDepth();

    DepthChecker* q;
    Element* m_target;
    int m_depth;
};

DepthChecker::DepthChecker(QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
}

DepthChecker::~DepthChecker()
{
}

int DepthChecker::depth() const
{
    return d->m_depth;
}

Element* DepthChecker::target() const
{
    return d->m_target;
}

void DepthChecker::setTarget(Element* target)
{
    if (d->m_target == target)
        return;

    if (d->m_target) {
        disconnect(d->m_target, SIGNAL(parentChanged(Element*)),
                   this, SLOT(updateDepth()));
    }

    d->m_target = target;

    if (d->m_target) {
        connect(d->m_target, SIGNAL(parentChanged(Element*)),
                this, SLOT(updateDepth()));
    }
    emit targetChanged(d->m_target);

    d->updateDepth();
}

void DepthChecker::Private::updateDepth()
{
    const int depth = elementDepth(m_target);
    if (m_depth == depth)
        return;

    m_depth = depth;
    emit q->depthChanged(m_depth);
}

#include "moc_depthchecker.cpp"
