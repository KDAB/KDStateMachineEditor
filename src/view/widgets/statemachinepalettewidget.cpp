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

#include "statemachinepalettewidget.h"

#include "element.h"
#include "kdsmeconstants.h"

#include "debug.h"
#include <QListView>
#include <QMimeData>
#include <QUrl>
#include <QRegularExpression>
#include <QVBoxLayout>

using namespace KDSME;

namespace KDSME {

class PaletteModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role
    {
        ElementTypeRole = Qt::UserRole + 1
    };

    explicit PaletteModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    struct Entry
    {
        Entry(Element::Type type_, const QString &iconName_, const QString &name_)
            : type(type_)
            , iconName(iconName_)
            , name(name_)
        {
        }
        Entry()
            : type(Element::ElementType)
        {
        }

        Element::Type type;
        QString iconName;
        QString name;
    };

    QVector<Entry> m_entries;
};

}

PaletteModel::PaletteModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_entries << Entry(Element::StateMachineType, QStringLiteral(":/kdsme/icons/state_machine.png"), tr("State Machine"));
    m_entries << Entry(Element::StateType, QStringLiteral(":/kdsme/icons/state.png"), tr("State"));
    m_entries << Entry(Element::FinalStateType, QStringLiteral(":/kdsme/icons/final_state.png"), tr("Final State"));
    m_entries << Entry(Element::HistoryStateType, QStringLiteral(":/kdsme/icons/shallow_history.png"), tr("History State"));

    m_entries << Entry(Element::SignalTransitionType, QStringLiteral(":/kdsme/icons/transition.png"), tr("Signal Transition"));
    m_entries << Entry(Element::TimeoutTransitionType, QStringLiteral(":/kdsme/icons/transition.png"), tr("Timeout Transition"));
}

int PaletteModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(m_entries.count());
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
        return QVariant();

    const Entry &entry = m_entries.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return entry.name;
    case Qt::DecorationRole: {
        return QIcon(entry.iconName);
    }
    case ElementTypeRole:
        return QVariant::fromValue<Element::Type>(entry.type);
    default:
        return QVariant();
    }
}

QMimeData *PaletteModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.size() == 1); // we only allow single selection

    const QModelIndex index = indexes.first();
    auto type = index.data(ElementTypeRole).value<Element::Type>();
    const QString typeString = QString::fromLatin1(Element::typeToString(type));

    auto *mimeData = new QMimeData;

    mimeData->setUrls({ QUrl(QStringLiteral("%1:Element/%2").arg(QStringLiteral(KDSME_QML_URI_PREFIX)).arg(typeString)) }); // clazy:exclude=qstring-arg

    // Following setData calls are used in QML DropArea.keys to accept/reject a drag and drop
    // depending on the data given. We are using this to allow for example "TransitionType"
    // elements not to be placed on a UmlStateMachine.qml but only "StateType" elements.

    static const QRegularExpression stateTypeRE(QStringLiteral(".+StateType$"));
    if (typeString.contains(stateTypeRE)) {
        mimeData->setData(QStringLiteral("StateType"), "");
    }

    static const QRegularExpression transitionTypeRE(QStringLiteral(".+TransitionType$"));
    if (typeString.contains(transitionTypeRE)) {
        mimeData->setData(QStringLiteral("TransitionType"), "");
    }

    mimeData->setData(QStringLiteral("external"), "");
    mimeData->setData(typeString, "");

    return mimeData;
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractListModel::flags(index);
    return Qt::ItemIsDragEnabled | flags;
}

struct StateMachinePaletteWidget::Private
{
};

StateMachinePaletteWidget::StateMachinePaletteWidget(QWidget *parent)
    : QWidget(parent)
    , d(new Private)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto *view = new QListView(this);
    view->setDragEnabled(true);
    view->setViewMode(QListView::IconMode);
    view->setFlow(QListView::LeftToRight);
    view->setGridSize(QSize(128, 48));
    view->setSpacing(10);
    view->setIconSize(QSize(32, 32));
    view->viewport()->setAcceptDrops(false); // disallow internal drops
    layout->addWidget(view);
    view->setModel(new PaletteModel);
}

StateMachinePaletteWidget::~StateMachinePaletteWidget()
{
}

#include "statemachinepalettewidget.moc"
