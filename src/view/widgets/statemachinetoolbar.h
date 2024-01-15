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

#ifndef KDSME_WIDGETS_STATEMACHINETOOLBAR_H
#define KDSME_WIDGETS_STATEMACHINETOOLBAR_H

#include "kdsme_view_export.h"

#include <QToolBar>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace KDSME {

class StateMachine;
class StateMachineView;

class KDSME_VIEW_EXPORT StateMachineToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit StateMachineToolBar(StateMachineView *view, QWidget *parent = nullptr);
    ~StateMachineToolBar();

private:
    struct Private;
    QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void handleExport())
};

}

#endif // STATEMACHINEVIEWTOOLBAR_H
