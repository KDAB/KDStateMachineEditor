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

#include "objecthelper.h"

#include <QMetaObject>
#include <QPointF>
#include <QRectF>
#include <QSizeF>

using namespace KDSME;

namespace {

QString stripNameSpace(const QString &className)
{
    const int pos = static_cast<int>(className.lastIndexOf(QLatin1String("::")));
    if (pos != -1)
        return className.mid(pos + 2);
    return className;
}
}

QString ObjectHelper::addressToString(const void *p)
{
    return (QLatin1String("0x") + QString::number(reinterpret_cast<quint64>(p), 16));
}

QString ObjectHelper::className(const QObject *object, ObjectHelper::DisplayOption option)
{
    QString className = QString::fromLatin1(object->metaObject()->className());
    return option == StripNameSpace ? stripNameSpace(className) : std::move(className);
}

QString ObjectHelper::displayString(const QObject *object, DisplayOption option)
{
    if (!object) {
        return QStringLiteral("QObject(0x0)");
    }
    if (object->objectName().isEmpty()) {
        return QString::fromLatin1("%1 (%2)").arg(addressToString(object)).arg(className(object, option)); // clazy:exclude=qstring-arg
    }
    return object->objectName();
}

const char *ObjectHelper::enumToString(const QMetaObject *metaObject, const char *name, int value)
{
    const int index = metaObject->indexOfEnumerator(name);
    const QMetaEnum metaEnum = metaObject->enumerator(index);
    return metaEnum.valueToKey(value);
}

int ObjectHelper::stringToEnum(const QMetaObject *metaObject, const char *name, const char *key)
{
    const int index = metaObject->indexOfEnumerator(name);
    const QMetaEnum metaEnum = metaObject->enumerator(index);
    return metaEnum.keyToValue(key);
}

QString ObjectHelper::toString(const QPointF &point) // clazy:exclude=function-args-by-value
{
    return QStringLiteral("(%1,%2)").arg(point.x()).arg(point.y());
}

QString ObjectHelper::toString(const QSizeF &size) // clazy:exclude=function-args-by-value
{
    return QStringLiteral("(%1,%2)").arg(size.width()).arg(size.height());
}

QString ObjectHelper::toString(const QRectF &rect)
{
    return QStringLiteral("(pos=%1,size=%2)")
        .arg(toString(rect.topLeft())) // clazy:exclude=qstring-arg
        .arg(toString(rect.size()));
}

bool ObjectHelper::descendantOf(const QObject *ascendant, const QObject *object)
{
    const QObject *parent = object->parent();
    if (!parent) {
        return false;
    }
    if (parent == ascendant) {
        return true;
    }
    return descendantOf(ascendant, parent);
}

int ObjectHelper::depth(const QObject *root, const QObject *object)
{
    if (!object)
        return -1;

    int depth = 0;
    const QObject *current = object;
    while (current && current != root) {
        ++depth;
        current = current->parent();
    }
    return depth;
}
