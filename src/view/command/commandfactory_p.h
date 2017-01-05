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

#ifndef COMMAND_COMMANDFACTORY_P_H
#define COMMAND_COMMANDFACTORY_P_H

#include "kdsme_view_export.h"

#include "element.h"

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
class StateModel;
class Transition;
class StateMachineScene;
}

class CommandFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateModel* model READ model WRITE setModel NOTIFY modelChanged)

public:
    explicit CommandFactory(QObject* parent = nullptr);

    KDSME::StateModel* model() const;
    void setModel(KDSME::StateModel* model);

    Q_INVOKABLE KDSME::CreateElementCommand* createElement(KDSME::StateModel* model, KDSME::Element::Type type, KDSME::Element* parentElement = nullptr) const;
    Q_INVOKABLE KDSME::DeleteElementCommand* deleteElement(KDSME::StateMachineScene* view, KDSME::Element* element) const;
    Q_INVOKABLE KDSME::LayoutSnapshotCommand* layoutSnapshot(KDSME::StateMachineScene* view, const QString& text = QString()) const;
    Q_INVOKABLE KDSME::ModifyPropertyCommand* modifyProperty(QObject* object, const char* property, const QVariant& value, const QString& text = QString());
    Q_INVOKABLE KDSME::ModifyPropertyCommand* modifyProperty(QObject* object, const QJsonObject& propertyMap, const QString& text = QString());
    Q_INVOKABLE KDSME::ModifyElementCommand* modifyElement(KDSME::Element* item);
    Q_INVOKABLE KDSME::ModifyTransitionCommand* modifyTransition(KDSME::Transition* transition, KDSME::StateModel *model);
    Q_INVOKABLE KDSME::ReparentElementCommand* reparentElement(KDSME::StateMachineScene* view, KDSME::Element* element);
    Q_INVOKABLE KDSME::ChangeStateMachineCommand* changeStateMachineElement(KDSME::StateMachineScene* view, KDSME::StateMachine* statemachine);

Q_SIGNALS:
    void modelChanged(KDSME::StateModel* model);

private:
    KDSME::StateModel* m_model;
};

Q_DECLARE_METATYPE(CommandFactory*)

#endif
