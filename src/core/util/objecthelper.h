/*
  objecthelper.h

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

#ifndef KDSME_UTIL_OBJECTHELPER_H
#define KDSME_UTIL_OBJECTHELPER_H

#include "kdsme_core_export.h"

#include <algorithm>

#include <QObject>
#include <QMetaEnum>
#include <QList>

class QPointF;

namespace KDSME {

namespace ObjectHelper {

    KDSME_CORE_EXPORT QString addressToString(const void *p);

    enum DisplayOption {
        NoStrip,
        StripNameSpace,
    };
    KDSME_CORE_EXPORT QString className(const QObject* object, DisplayOption option = NoStrip);
    KDSME_CORE_EXPORT QString displayString(const QObject *object, DisplayOption option = NoStrip);

    KDSME_CORE_EXPORT const char* enumToString(const QMetaObject* metaObject, const char* name, int value);
    KDSME_CORE_EXPORT int stringToEnum(const QMetaObject* metaObject, const char* name, const char* key);

    KDSME_CORE_EXPORT QString toString(const QPointF& point);
    KDSME_CORE_EXPORT QString toString(const QSizeF& size);
    KDSME_CORE_EXPORT QString toString(const QRectF& rect);

    /**
     * Filters the list @p list
     *
     * Returns a list that only contains item of type @p FilterType
     */
    template<class FilterType, class ItemType>
    QList<FilterType> copy_if_type(const QList<ItemType> list) {
        QList<FilterType> filteredList;
        Q_FOREACH (const ItemType object, list) {
            if (FilterType filterObject = qobject_cast<FilterType>(object))
                filteredList << filterObject;
        }
        return filteredList;
    }

    /**
     * Filter out each item in @p source for that @p predicate is true
     *
     * Returns a *copy* of the original container @p source
     */
    template<class C, class P>
    C filter(const C& source, const P& predicate)
    {
        C d = source;
        // filter elements
        d.erase(std::remove_if(std::begin(d), std::end(d), predicate), std::end(d));
        return d;
    }

    /**
     * Finds the parent QObject of the specified type T, if such exists.
     * @param object is a pointer to a QObject.
     *
     * @return zero on failure; else a pointer to a data type T.
     *
     */
    template <typename T>
    T *findParentOfType(QObject *object) {
        if (!object) {
            return nullptr;
        }
        if (qobject_cast<T*>(object)) {
            return static_cast<T*>(object);
        }
        return findParentOfType<T>(object->parent());
    }

}

}

#endif
