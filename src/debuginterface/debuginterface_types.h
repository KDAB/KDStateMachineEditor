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

#ifndef KDSME_DEBUGINTERFACE_TYPES_H
#define KDSME_DEBUGINTERFACE_TYPES_H

#include <QDataStream>
#include <QList>
#include <QMetaType>

QT_BEGIN_NAMESPACE
class QAbstractTransition;
class QAbstractState;
QT_END_NAMESPACE

namespace KDSME {
namespace DebugInterface {

// note: typedef bring major pain, on the client side i.e. it would always look for
// signal/slots with the base type (or actually, the first type which was registered
// to the meta type system)...
struct TransitionId
{
    operator quint64() const
    {
        return id;
    }
    quint64 id;
};

inline QDataStream &operator<<(QDataStream &out, TransitionId value)
{
    out << value.id;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, TransitionId &value)
{
    in >> value.id;
    return in;
}

struct StateId
{
    operator quint64() const
    {
        return id;
    }
    quint64 id;
};

inline QDataStream &operator<<(QDataStream &out, StateId value)
{
    out << value.id;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, StateId &value)
{
    in >> value.id;
    return in;
}

enum StateType {
    OtherState,
    FinalState,
    ShallowHistoryState,
    DeepHistoryState,
    StateMachineState
};

inline QDataStream &operator<<(QDataStream &out, StateType value)
{
    out << int(value);
    return out;
}

inline QDataStream &operator>>(QDataStream &in, StateType &value)
{
    int val;
    in >> val;
    value = static_cast<StateType>(val);
    return in;
}

typedef QList<StateId> StateMachineConfiguration;

inline void registerTypes() //krazy:exclude=inline
{
    qRegisterMetaTypeStreamOperators<StateId>();
    qRegisterMetaTypeStreamOperators<StateMachineConfiguration>();
    qRegisterMetaTypeStreamOperators<TransitionId>();
    qRegisterMetaTypeStreamOperators<StateType>();
}

}
}

Q_DECLARE_METATYPE(KDSME::DebugInterface::StateId)
Q_DECLARE_METATYPE(KDSME::DebugInterface::TransitionId)
Q_DECLARE_METATYPE(KDSME::DebugInterface::StateMachineConfiguration)
Q_DECLARE_METATYPE(KDSME::DebugInterface::StateType)

#endif
