/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "debug.h"
#include "statemachineview.h"

#include <QtTest>

using namespace KDSME;

class StateMachineViewTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testEmpty();
};

void StateMachineViewTest::testEmpty()
{
    // This currently crashes when StateMachineView is being destroyed,
    // because of https://bugreports.qt.io/browse/QTBUG-40745
    // TODO: If the issue keeps being considered an "invalid" use-case,
    // we'll have to refactor StateMachineView and get rid off the self reference as context property.
    //StateMachineView view;
}

QTEST_MAIN(StateMachineViewTest)

#include "test_statemachineview.moc"
