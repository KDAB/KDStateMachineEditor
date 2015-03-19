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

#ifndef KDSME_COMMAND_MODIFYPROPERTYCOMMAND_P_H
#define KDSME_COMMAND_MODIFYPROPERTYCOMMAND_P_H

#include "command_p.h"

#include <QHash>
#include <QPointer>

class QJsonObject;
class QVariant;

namespace KDSME {

/**
 * @brief Modifies any specific property of a @ref QObject
 *
 * Properties can also be referenced by their name (read: a simple string),
 * so we can have a generic command that is capable of setting
 * individual properties of an object.
 * This is sometimes useful in order to not have to create commands
 * for each and every property modification.
 */
class KDSME_VIEW_EXPORT ModifyPropertyCommand : public KDSME::Command
{
    Q_OBJECT

public:
    ModifyPropertyCommand(QObject* object, const char* property, const QVariant& value, const QString& text = QString(), QUndoCommand* parent = nullptr);
    ModifyPropertyCommand(QObject* object, const QJsonObject& propertyMap, const QString& text = QString(), QUndoCommand* parent = nullptr);

    virtual int id() const override { return ModifyProperty; }

    virtual void redo() override;
    virtual void undo() override;

private:
    void init();

    QPointer<QObject> m_object;
    QHash<QByteArray, QVariant> m_propertyMap;
    QHash<QByteArray, QVariant> m_oldPropertyMap;
};

}

#endif // MODIFYPROPERTYCOMMAND_P_H
