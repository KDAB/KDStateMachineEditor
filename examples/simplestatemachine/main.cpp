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

#include "config-examples.h"

#include <state.h>
#include <statemachinescene.h>
#include <statemachineview.h>
#include <transition.h>

#include <QApplication>
#include <QDebug>

using namespace KDSME;

/**
 * @brief Instantiating a state chart by hand and displaying it
 */
int main(int argc, char **argv)
{
    const QApplication app(argc, argv);

    //! [Display the state machine]
    StateMachine machine;
    State s1(&machine);
    s1.setLabel(QStringLiteral("s1"));
    State s2(&machine);
    s2.setLabel(QStringLiteral("s2"));
    s1.addSignalTransition(&s2);

    StateMachineView view;
    view.scene()->setRootState(&machine);
    view.scene()->layout();
    view.resize(800, 600);
    view.show();
    //! [Display the state machine]

    QApplication::exec();
}
