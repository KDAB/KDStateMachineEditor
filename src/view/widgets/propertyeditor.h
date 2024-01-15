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

#ifndef KDSME_WIDGETS_PROPERTYEDITOR_H
#define KDSME_WIDGETS_PROPERTYEDITOR_H

#include "kdsme_view_export.h"

#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QModelIndex;
QT_END_NAMESPACE

namespace KDSME {

class CommandController;
class Element;
class StateModel;

class KDSME_VIEW_EXPORT PropertyEditor : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent = nullptr);
    ~PropertyEditor();

    void setSelectionModel(QItemSelectionModel *selectionModel);
    void setCommandController(CommandController *cmdController);
    void setStateModel(StateModel *selectionModel);

private:
    struct Private;
    QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void updateSimpleProperty())
    Q_PRIVATE_SLOT(d, void setInitalState(const QString &label))
    Q_PRIVATE_SLOT(d, void setDefaultState(const QString &label))
    Q_PRIVATE_SLOT(d, void setSourceState(const QString &label))
    Q_PRIVATE_SLOT(d, void setTargetState(const QString &label))
    Q_PRIVATE_SLOT(d, void childModeChanged())
    Q_PRIVATE_SLOT(d, void currentChanged(const QModelIndex &current, const QModelIndex &previous))
    Q_PRIVATE_SLOT(d, void modelAboutToBeReset())
    Q_PRIVATE_SLOT(d, void loadFromCurrentElement())
};

} // namespace KDSME

#endif // PROPERTYEDITOR_H
