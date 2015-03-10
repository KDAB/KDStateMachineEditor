/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "trafficlight.h"

#include <state.h>
#include <statemachinescene.h>
#include <statemachineview.h>
#include <qsmadapter.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFinalState>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScopedPointer>
#include <QState>
#include <QStateMachine>
#include <QRemoteObjectNode>

using namespace KDSME;

/**
 * @brief Create a GUI driven by a state machine, realtime-debug this state machine in another window
 */
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // setup debuggee
    TrafficLight trafficLight;
    trafficLight.resize(110, 300);
    trafficLight.show();

    StateMachineView view;
    view.resize(800, 600);
    view.show();

    QsmAdapter adapter;
    QObject::connect(&adapter, &QsmAdapter::repopulateView,
                     [&]() {
            qDebug() << "Updating state machine in view";
            view.scene()->setRootState(adapter.machine());
            view.scene()->layout();
        }
    );
    adapter.setQStateMachine(trafficLight.machine());

    app.exec();
}
