/*
  objecthelper.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "objecthelper.h"

#include <QMetaObject>
#include <QPointF>
#include <QRectF>
#include <QSizeF>

using namespace KDSME;

namespace {

QString stripNameSpace(const QString& className)
{
    const int pos = className.lastIndexOf("::");
    if (pos != -1)
        return className.mid(pos + 2);
    return className;
}
}

QString ObjectHelper::addressToString(const void* p)
{
    return (QLatin1String("0x") + QString::number(reinterpret_cast<quint64>(p), 16));
}

QString ObjectHelper::className(const QObject* object, ObjectHelper::DisplayOption option)
{
    return (option == StripNameSpace ?
            stripNameSpace(object->metaObject()->className()) : object->metaObject()->className());
}

QString ObjectHelper::displayString(const QObject* object, DisplayOption option)
{
    if (!object) {
        return "QObject(0x0)";
    }
    if (object->objectName().isEmpty()) {
        return QString::fromLatin1("%1 (%2)").
            arg(addressToString(object)).
            arg(className(object, option));
    }
    return object->objectName();
}

const char* ObjectHelper::enumToString(const QMetaObject* metaObject, const char* name, int value)
{
    const int index = metaObject->indexOfEnumerator(name);
    const QMetaEnum metaEnum = metaObject->enumerator(index);
    return metaEnum.valueToKey(value);
}

int ObjectHelper::stringToEnum(const QMetaObject* metaObject, const char* name, const char* key)
{
    const int index = metaObject->indexOfEnumerator(name);
    const QMetaEnum metaEnum = metaObject->enumerator(index);
    return metaEnum.keyToValue(key);
}

QString ObjectHelper::toString(const QPointF& point)
{
    return QString("(%1,%2)").arg(point.x()).arg(point.y());
}

QString ObjectHelper::toString(const QSizeF& size)
{
    return QString("(%1,%2)").arg(size.width()).arg(size.height());
}

QString ObjectHelper::toString(const QRectF& rect)
{
    return QString("(pos=%1,size=%2)")
            .arg(toString(rect.topLeft()))
            .arg(toString(rect.size()));
}
