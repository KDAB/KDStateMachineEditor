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

#include "debug.h"
#include "statemachineview.h"

#include <QTest>

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
    // StateMachineView view;
}

QTEST_MAIN(StateMachineViewTest)

#include "test_statemachineview.moc"
