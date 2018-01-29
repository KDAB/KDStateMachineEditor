/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2015-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_DEPTHCHECKER_H
#define KDSME_DEPTHCHECKER_H

#include "kdsme_core_export.h"

#include "element.h"

#include <QObject>

namespace KDSME {

class KDSME_CORE_EXPORT DepthChecker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KDSME::Element* target READ target WRITE setTarget NOTIFY targetChanged FINAL)
    Q_PROPERTY(int depth READ depth NOTIFY depthChanged FINAL)

public:
    explicit DepthChecker(QObject* parent = nullptr);
    ~DepthChecker();

    Element* target() const;
    void setTarget(Element* target);

    int depth() const;

Q_SIGNALS:
    void targetChanged(QObject* target);
    void depthChanged(int depth);

private:
    struct Private;
    QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void updateDepth())
};

}

#endif // KDSME_DEPTHCHECKER_H
