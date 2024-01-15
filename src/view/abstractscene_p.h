/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_VIEW_ABSTRACTSCENE_P_H
#define KDSME_VIEW_ABSTRACTSCENE_P_H

#include "abstractscene.h"

namespace KDSME {

class AbstractScenePrivate
{
public:
    explicit AbstractScenePrivate(AbstractScene *qq);

    static AbstractScenePrivate *get(AbstractScene *q)
    {
        return q->d.data();
    }

    AbstractScene *q;
    QAbstractItemModel *m_model;
    QPointer<QItemSelectionModel> m_selectionModel;
    QQuickItem *m_instantiator;
    AbstractScene::EditTriggers m_editTriggers;
    Qt::ContextMenuPolicy m_contextMenuPolicy = Qt::DefaultContextMenu;
    AbstractScene::ViewState m_state;
};

}

#endif
