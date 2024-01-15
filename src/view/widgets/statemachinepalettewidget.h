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

#ifndef KDSME_WIDGETS_STATEMACHINEPALETTEWIDGET_H
#define KDSME_WIDGETS_STATEMACHINEPALETTEWIDGET_H

#include "kdsme_view_export.h"

#include <QWidget>

namespace KDSME {

class KDSME_VIEW_EXPORT StateMachinePaletteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StateMachinePaletteWidget(QWidget *parent = nullptr);
    ~StateMachinePaletteWidget();

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif // STATEMACHINEPALETTEWIDGET_H
