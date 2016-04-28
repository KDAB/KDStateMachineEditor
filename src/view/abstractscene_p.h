#ifndef KDSME_VIEW_ABSTRACTSCENE_P_H
#define KDSME_VIEW_ABSTRACTSCENE_P_H

#include "abstractscene.h"

namespace KDSME {

class AbstractScenePrivate
{
public:
    AbstractScenePrivate(AbstractScene* q);

    static AbstractScenePrivate* get(AbstractScene* q) { return q->d.data(); }

    AbstractScene* q;
    QAbstractItemModel* m_model;
    QPointer<QItemSelectionModel> m_selectionModel;
    QQuickItem* m_instantiator;
    AbstractScene::EditTriggers m_editTriggers;
    Qt::ContextMenuPolicy m_contextMenuPolicy = Qt::DefaultContextMenu;
    AbstractScene::ViewState m_state;
};

}

#endif
