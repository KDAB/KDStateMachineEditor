/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    //! [Display the state machine]
    StateMachine machine;
    State s1(&machine);
    s1.setLabel("s1");
    State s2(&machine);
    s2.setLabel("s2");
    s1.addSignalTransition(&s2);

    StateMachineView view;
    view.scene()->setRootState(&machine);
    view.scene()->layout();
    view.resize(800, 600);
    view.show();
    //! [Display the state machine]

    app.exec();
}
