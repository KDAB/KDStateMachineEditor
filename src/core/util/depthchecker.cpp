/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "depthchecker.h"

using namespace KDSME;

namespace {

int elementDepth(const Element *element)
{
    if (!element)
        return -1;

    int depth = 0;
    const Element *current = element;
    while (current) {
        ++depth;
        current = current->parentElement();
    }
    return depth;
}

}

struct DepthChecker::Private
{
    Private(DepthChecker *q)
        : q(q)
        , m_target(nullptr)
        , m_depth(-1)
    {
    }

    void updateDepth();

    DepthChecker *q;
    Element *m_target;
    int m_depth;
};

DepthChecker::DepthChecker(QObject *parent)
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

Element *DepthChecker::target() const
{
    return d->m_target;
}

void DepthChecker::setTarget(Element *target)
{
    if (d->m_target == target)
        return;

    if (d->m_target) {
        // clang-format off
        disconnect(d->m_target, SIGNAL(parentChanged(Element*)), // clazy:exclude=old-style-connect
                   this, SLOT(updateDepth()));
        // clang-format on
    }

    d->m_target = target;

    if (d->m_target) {
        // clang-format off
        connect(d->m_target, SIGNAL(parentChanged(Element*)), // clazy:exclude=old-style-connect
                this, SLOT(updateDepth()));
        // clang-format on
    }
    Q_EMIT targetChanged(d->m_target);

    d->updateDepth();
}

void DepthChecker::Private::updateDepth()
{
    const int depth = elementDepth(m_target);
    if (m_depth == depth)
        return;

    m_depth = depth;
    Q_EMIT q->depthChanged(m_depth);
}

#include "moc_depthchecker.cpp"
