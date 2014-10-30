/*
  editcontroller.cpp

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

#include "editcontroller.h"
#include "widgets/statemachineview.h"

#include "createelementcommand.h"
#include "command.h"
#include "element.h"
#include "layoutitem.h"
#include "layoutitemmodel.h"
#include "layoutimportexport.h"
#include "kdsmeconstants.h"
#include "modifylayoutitemcommand.h"
#include "view.h"

#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

using namespace KDSME;

struct EditController::Private
{
    Private();

    bool m_editModeEnabled;
};

EditController::Private::Private()
    : m_editModeEnabled(false)
{

}

EditController::EditController(StateMachineView* parent)
    : AbstractController(parent)
    , d(new Private)
{
}

EditController::~EditController()
{
}

bool EditController::editModeEnabled() const
{
    return d->m_editModeEnabled;
}

void EditController::setEditModeEnabled(bool editModeEnabled)
{
    if (d->m_editModeEnabled == editModeEnabled)
        return;

    d->m_editModeEnabled = editModeEnabled;
    emit editModeEnabledChanged(d->m_editModeEnabled);
}

bool EditController::sendDragEnterEvent(LayoutItem* sender, LayoutItem* target, const QPoint& pos, const QList<QUrl>& urls)
{
    Q_UNUSED(pos);

    qDebug() << Q_FUNC_INFO << "sender=" << sender << "target=" << target << "pos=" << pos << "urls=" << urls;

    // For the case a TransitionLayoutItem is dragged onto a StateLayoutItem that
    // StateLayoutItem will turns into the new source/target of the transition.
    if (qobject_cast<TransitionLayoutItem*>(sender)) {
        return qobject_cast<StateLayoutItem*>(target);
    }

    // No sender means we expect a URL to be given.
    if (urls.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "No urls";
        return false;
    }

    // we only accept one item only for now
    const QUrl url = urls.first();
    if (url.scheme() != KDSME_QML_URI_PREFIX) {
        qDebug()<< Q_FUNC_INFO << "Unexpected Url Schema=" << url.scheme();
        return false;
    }

    return true;
}

bool EditController::sendDropEvent(LayoutItem* sender, LayoutItem* target, const QPoint& pos, const QList<QUrl>& urls)
{
    Q_UNUSED(sender);
    Q_UNUSED(pos);

    qDebug() << Q_FUNC_INFO << "sender=" << sender << "target=" << target << "pos=" << pos << "urls=" << urls;

    if (urls.isEmpty()) {
        qDebug()<< Q_FUNC_INFO << "No urls";
        return false;
    }

    // we only accept one item only for now
    const QUrl url = urls.first();
    if (url.scheme() != KDSME_QML_URI_PREFIX) {
        qDebug()<< Q_FUNC_INFO << "Unexpected Url Schema=" << url.scheme();
        return false;
    }

    const QString str = url.toString(QUrl::RemoveScheme);
    const QString typeString = str.split('/').last();
    if (typeString.isEmpty())
        return false;

    Element::Type type = Element::stringToType(qPrintable(typeString));

    // TODO should we probably either move that command to kdstatemachine/commands
    // for reuse or even extend the existing CreateElementCommand to set optionally
    // an initial position/geometry?
    class CreateAndPositionCommand : public Command {
    public:
        CreateAndPositionCommand(View *view, Element::Type type, Element *targetElement, const QPointF &pos)
            : Command(view->stateModel())
            , m_view(view)
            , m_createcmd(new CreateElementCommand(view->stateModel(), type))
            , m_pos(pos)
        {
            m_createcmd->setParentElement(targetElement);
            setText(m_createcmd->text());
        }
        virtual void redo()
        {
            // save the current layout
            Q_ASSERT(m_view->rootLayoutItem());
            const QJsonDocument doc(LayoutImportExport::exportLayout(m_view->rootLayoutItem()));

            m_createcmd->redo();

            Element *element = m_createcmd->createdElement();
            if (!element) // creating the element failed, abort here
                return;

            // re-import is needed so the newly created element gets a LayoutItem
            m_view->import();

            // restore the previous layout
            Q_ASSERT(m_view->rootLayoutItem());
            LayoutImportExport::importLayout(doc.object(), m_view->rootLayoutItem());

            // move the new element to its position and set a sane initial size
            LayoutItem* layoutitem = m_view->layoutItemForElement(element);
            Q_ASSERT(layoutitem);
            ModifyLayoutItemCommand poscmd(layoutitem);
            QPointF pos = m_pos;
            QSizeF size = layoutitem->preferredSize();
            if (size.width() > 0)
                pos.setX(qMax<qreal>(0, pos.x() - size.width()/2));
            if (size.height() > 0)
                pos.setY(qMax<qreal>(0, pos.y() - size.height()/2));
            poscmd.setGeometry(QRectF(pos, size));
            poscmd.redo();

            // TODO this rearranges the just created and positioned element, why?
            //m_view->layout();

            // Mark the new LayoutItem as current one what means the item is selected
            // as if a user clicked on it.
            m_view->setCurrentItem(layoutitem);
        }
        virtual void undo() {
            m_createcmd->undo();
        }
    private:
        View *m_view;
        QScopedPointer<CreateElementCommand> m_createcmd;
        QPointF m_pos;
    };

    Element *targetElement = target ? target->element() : 0;
    // TODO: Try to decouple more
    auto view = stateMachineView()->view();
    CreateAndPositionCommand *cmd = new CreateAndPositionCommand(view, type, targetElement, QPointF(pos));
    stateMachineView()->sendCommand(cmd);

    return true;
}
