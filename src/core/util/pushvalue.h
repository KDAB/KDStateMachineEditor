/*
  pushvalue.h

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

#ifndef KDSME_UTIL_PUSHVALUE_H
#define KDSME_UTIL_PUSHVALUE_H

/**
 * A simple helper-class that does the following:
 * Backup the given reference-value given through @param ptr,
 * replace it with the value given through @param push,
 * restore the backed up value back on destruction.
 * */
template<class Value>
class PushValue
{
public:
    inline PushValue(Value* ref, const Value& newValue = Value())
        : m_ref(ref)
        , m_oldValue(*ref)
    {
        *m_ref = newValue;
    }

    inline ~PushValue()
    {
        *m_ref = m_oldValue;
    }

private:
    Value* m_ref;
    Value m_oldValue;
};

class BoolLocker : public PushValue<bool>
{
public:
    /**
     * The value in @p ref will be set to true on construction of BoolLocker
     * and is set to false when the BoolLocker object is destroyed
     */
    inline explicit BoolLocker(bool* ref)
        : PushValue<bool>(ref, true) {}
};

#endif
