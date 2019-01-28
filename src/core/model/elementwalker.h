/*
  elementwalker.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_MODEL_ELEMENTWALKER_H
#define KDSME_MODEL_ELEMENTWALKER_H

#include "element.h"
#include "treewalker.h"

namespace KDSME {

template<>
struct TreeWalkerTrait<Element*>
{
    static inline QList<Element*> children(Element* element)
    {
        return element->childElements();
    }
};
typedef TreeWalker<Element*> ElementWalker;

}

#endif
