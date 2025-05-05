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

// krazy:excludeall=inline as this is a template
#ifndef KDSME_UTIL_RINGBUFFER_H
#define KDSME_UTIL_RINGBUFFER_H

#include <QQueue>
#include <QList>

namespace KDSME {

/**
 * This class is similar to a QQueue but provides circular semantics.
 * For example, appending to an already "full" queue will overwrite the
 * oldest item, i.e. it forms a kind of LRU cache.
 *
 * Or, in other words, RingBuffer is a QQueue with limited capacity.
 *
 * The API is essentially simplified version of QVector, which additional methods
 * such as @ref enqueue(), @ref setCapacity()
 *
 * @sa QVector
 */
template<class T>
class RingBuffer
{
public:
    /**
     * Construct a ring buffer with initial capacity @p capacity
     */
    explicit RingBuffer(int capacity = 10)
        : m_capacity(capacity)
    {
    }

    /**
     * (Re-)set the capacity of this ring buffer to @p capacity
     *
     * @note Will run cleanup() afterwards and remove items if capacity decreased
     * @sa cleanup()
     */
    void setCapacity(int capacity)
    {
        Q_ASSERT(capacity > 0);
        m_capacity = capacity;
        cleanup();
    }

    /**
     * Adds value @p t to the tail of the queue.
     */
    void enqueue(T t)
    {
        m_entries.enqueue(t);
        cleanup();
    }

    /**
     * Clear the buffer
     */
    void clear()
    {
        m_entries.clear();
    }

    inline const T &at(int i) const
    {
        return m_entries.at(i);
    }
    inline int size() const
    {
        return m_entries.size();
    }
    /**
     * Returns a reference to the queue's head item.
     * This function assumes that the queue isn't empty.
     */
    inline T head() const
    {
        return m_entries.head();
    }
    /**
     * Returns a reference to the last item in the list.
     * The list must not be empty.
     */
    inline T last() const
    {
        return m_entries.last();
    }
    inline QList<T> entries() const
    {
        return m_entries;
    }

private:
    void cleanup()
    {
        while (m_entries.size() > m_capacity) {
            m_entries.dequeue();
        }
    }

    QQueue<T> m_entries;
    int m_capacity;
};

}

#endif
