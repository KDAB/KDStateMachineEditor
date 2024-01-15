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

#ifndef COMMAND_COMMANDFACTORY_P_H
#define COMMAND_COMMANDFACTORY_P_H

#include "kdsme_view_export.h"

#include "element.h"
#include "elementmodel.h"

#include <QObject>

namespace KDSME {
class CreateElementCommand;
class DeleteElementCommand;
class LayoutSnapshotCommand;
class ModifyPropertyCommand;
class ModifyElementCommand;
class ModifyTransitionCommand;
class ReparentElementCommand;
class ChangeStateMachineCommand;
class Transition;
class StateMachineScene;
}

class CommandFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateModel *model READ model WRITE setModel NOTIFY modelChanged)

public:
    explicit CommandFactory(QObject *parent = nullptr);

    KDSME::StateModel *model() const;
    void setModel(KDSME::StateModel *model);

    Q_INVOKABLE KDSME::CreateElementCommand *createElement(KDSME::StateModel *model, KDSME::Element::Type type, KDSME::Element *parentElement = nullptr) const;
    Q_INVOKABLE KDSME::DeleteElementCommand *deleteElement(KDSME::StateMachineScene *view, KDSME::Element *element) const;
    Q_INVOKABLE KDSME::LayoutSnapshotCommand *layoutSnapshot(KDSME::StateMachineScene *view, const QString &text = QString()) const;
    Q_INVOKABLE KDSME::ModifyPropertyCommand *modifyProperty(QObject *object, const char *property, const QVariant &value, const QString &text = QString());
    Q_INVOKABLE KDSME::ModifyPropertyCommand *modifyProperty(QObject *object, const QJsonObject &propertyMap, const QString &text = QString());
    Q_INVOKABLE KDSME::ModifyElementCommand *modifyElement(KDSME::Element *item);
    Q_INVOKABLE KDSME::ModifyTransitionCommand *modifyTransition(KDSME::Transition *transition, KDSME::StateModel *model);
    Q_INVOKABLE KDSME::ReparentElementCommand *reparentElement(KDSME::StateMachineScene *view, KDSME::Element *element);
    Q_INVOKABLE KDSME::ChangeStateMachineCommand *changeStateMachineElement(KDSME::StateMachineScene *view, KDSME::StateMachine *statemachine);

Q_SIGNALS:
    void modelChanged(KDSME::StateModel *model);

private:
    KDSME::StateModel *m_model;
};

Q_DECLARE_METATYPE(CommandFactory *)

#endif
