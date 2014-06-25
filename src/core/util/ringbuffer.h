/*
  ringbuffer.h

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

#ifndef KDSME_UTIL_RINGBUFFER_H
#define KDSME_UTIL_RINGBUFFER_H

#include <QQueue>
#include <QList>

template<class T>
class RingBuffer
{
  public:
    RingBuffer(int capacity = 10) : m_capacity(capacity) {}

    void setCapacity(int capacity)
    {
        Q_ASSERT(capacity > 0);
        m_capacity = capacity;
        cleanup();
    }

    void enqueue(T t)
    {
        m_entries.enqueue(t);
        cleanup();
    }

    void clear()
    {
        m_entries.clear();
    }

    inline const T& at(int i) const { return m_entries.at(i); }
    inline int size() const { return m_entries.size(); }
    inline T head() const { return m_entries.head(); }
    inline T last() const { return m_entries.last(); }
    inline QList<T> entries() const { return m_entries; }

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

#endif
