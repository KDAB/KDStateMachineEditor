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

#ifndef KDSME_UTIL_TREEWALKER_H
#define KDSME_UTIL_TREEWALKER_H

#include "kdsme_core_export.h"

#include <QObject>

#include <functional>

namespace KDSME {

template<typename T>
struct TreeWalkerTrait
{
    static QList<T> children(T item)
    {
        Q_UNUSED(item);
        return QList<T>();
    }
};

/**
 * Performs a DFS walk through the hierarchy of T
 *
 * @note Specialize TreeWalkerTrait for your type T to get TreeWalker support
 */
template<typename T>
class TreeWalker
{
public:
    enum VisitResult
    {
        ContinueWalk, ///< Continues traversal with the next sibling of the item just visited, without visiting its children
        RecursiveWalk, ///< Traverse the children of this item
        StopWalk ///< Terminate the traversal
    };

    enum TraversalType
    {
        PreOrderTraversal,
        PostOrderTraversal,
    };

    typedef std::function<VisitResult(T)> VisitFunction;

    explicit TreeWalker(TraversalType type = PreOrderTraversal)
        : m_traversalType(type)
    {
    }

    /**
     * Walk through all items including the start item itself
     *
     * @return True in case we walked through all items, false otherwise
     *
     * @param item the start item
     * @param visit Function called each time a LayoutItem instance is encountered
     */
    bool walkItems(T item, const VisitFunction &visit)
    {
        if (!item)
            return false;

        bool continueWalk = true;
        if (m_traversalType == PreOrderTraversal) {
            continueWalk = (visit(item) == TreeWalker::RecursiveWalk);
        }
        Q_ASSERT(item);
        const auto items = TreeWalkerTrait<T>::children(item);
        if (std::any_of(items.begin(), items.end(),
                        [&](const T &child) { return !walkItems(child, visit); })) {
            return false;
        }

        if (m_traversalType == PostOrderTraversal) {
            continueWalk = (visit(item) == TreeWalker::RecursiveWalk);
        }
        return continueWalk;
    }

    /**
     * Convenience function. Same as walk(), but omits item @p item
     *
     * @sa walk()
     */
    bool walkChildren(T item, const VisitFunction &visit)
    {
        if (!item)
            return false;

        const auto items = TreeWalkerTrait<T>::children(item);
        return !std::any_of(items.begin(), items.end(),
                            [&](const T &child) { return !walkItems(child, visit); });
    }

private:
    TraversalType m_traversalType;
};

}

#endif // TREEWALKER_H
