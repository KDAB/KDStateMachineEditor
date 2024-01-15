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

#ifndef KDSME_LAYOUT_LAYOUTIMPORTEXPORT_H
#define KDSME_LAYOUT_LAYOUTIMPORTEXPORT_H

#include "kdsme_core_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace KDSME {

class State;

/**
 * @brief Utils for importing/exporting layout properties of a the object
 * tree representing a state
 */
namespace LayoutImportExport {
/**
 * @brief Check if the ids in @p data still match the ids from the
 * object tree represented by @p state
 *
 * @return True if ids are identical, otherwise false
 */
KDSME_CORE_EXPORT bool matches(const QJsonObject &data, State *state);

/**
 * @brief Export layout of @p state into a machine-parsable JSON format
 *
 * Goes through the object tree representented by @p state and fetches
 * the layout properties of each element
 * (such as, x/y-position, width/height, shape, ...)
 *
 * The resulting JSON output can be used to restore the layout of the exact
 * same state hierarchy
 *
 * @sa importLayout()
 */
KDSME_CORE_EXPORT QJsonObject exportLayout(const State *state);

/**
 * @brief Import layout @p data to set properties of @p state and its children
 *
 * @note Ids of @p state and its children must match with the ids
 * stored in @p data, otherwise the layout cannot be imported
 *
 * @sa matches()
 * @sa exportLayout()
 */
KDSME_CORE_EXPORT void importLayout(const QJsonObject &data, State *state);
}

}

/**
 * @example layoutexport/main.cpp
 *
 * This is an example of how to use LayoutImportExport::exportLayout()
 * and the resulting JSON it produces
 */

#endif // LAYOUTIMPORTEXPORT_H
