/*
  command.cpp

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

#include "command.h"

#include "elementmodel.h"

using namespace KDSME;

Command::Command(KDSME::StateModel* model, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_model(model)
{
}

Command::Command(const QString& text, QUndoCommand* parent)
    : QUndoCommand(text, parent)
    , m_model(nullptr)
{

}

KDSME::StateModel* Command::model() const
{
    return m_model;
}
