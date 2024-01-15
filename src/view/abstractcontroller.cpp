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

#include "abstractcontroller.h"

using namespace KDSME;

struct AbstractController::Private
{
    Private()
        : m_view(nullptr)
    {
    }

    StateMachineView *m_view;
};

AbstractController::AbstractController(StateMachineView *view)
    : QObject(view)
    , d(new Private)
{
    d->m_view = view;
}

AbstractController::~AbstractController()
{
}

StateMachineView *AbstractController::stateMachineView() const
{
    return d->m_view;
}
