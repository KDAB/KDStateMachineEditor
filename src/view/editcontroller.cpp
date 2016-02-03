/*
  editcontroller.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "createelementcommand_p.h"
#include "command_p.h"
#include "state.h"
#include "transition.h"
#include "layoutimportexport.h"
#include "kdsmeconstants.h"
#include "modifyelementcommand_p.h"
#include "statemachinescene.h"

#include <QUrl>
#include "debug.h"
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

bool EditController::sendDragEnterEvent(Element* sender, Element* target, const QPoint& pos, const QList<QUrl>& urls)
{
    Q_UNUSED(pos);

    qCDebug(KDSME_VIEW) << "sender=" << sender << "target=" << target << "pos=" << pos << "urls=" << urls;

    // For the case a Transition is dragged onto a State that
    // State will turns into the new source/target of the transition.
    if (qobject_cast<Transition*>(sender)) {
        return qobject_cast<State*>(target);
    }

    // No sender means we expect a URL to be given.
    if (urls.isEmpty()) {
        qCDebug(KDSME_VIEW) << "No urls";
        return false;
    }

    // we only accept one item only for now
    const QUrl url = urls.first();
    if (url.scheme() != KDSME_QML_URI_PREFIX) {
        qCDebug(KDSME_VIEW)<< "Unexpected Url Schema=" << url.scheme();
        return false;
    }

    return true;
}

bool EditController::sendDropEvent(Element* sender, Element* target, const QPoint& pos, const QList<QUrl>& urls)
{
    Q_UNUSED(sender);
    Q_UNUSED(pos);

    qCDebug(KDSME_VIEW) << "sender=" << sender << "target=" << target << "pos=" << pos << "urls=" << urls;

    if (urls.isEmpty()) {
        qCDebug(KDSME_VIEW)<< "No urls";
        return false;
    }

    // we only accept one item only for now
    const QUrl url = urls.first();
    if (url.scheme() != KDSME_QML_URI_PREFIX) {
        qCDebug(KDSME_VIEW)<< "Unexpected Url Schema=" << url.scheme();
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
        CreateAndPositionCommand(StateMachineScene *view, Element::Type type, Element *targetElement, const QPointF &pos)
            : Command(view->stateModel())
            , m_view(view)
            , m_createcmd(new CreateElementCommand(view->stateModel(), type))
            , m_pos(pos)
        {
            m_createcmd->setParentElement(targetElement);
            setText(m_createcmd->text());
        }
        void redo() override
        {
            // save the current layout
            Q_ASSERT(m_view->rootState());
            const QJsonDocument doc(LayoutImportExport::exportLayout(m_view->rootState()));

            m_createcmd->redo();

            Element *element = m_createcmd->createdElement();
            if (!element) // creating the element failed, abort here
                return;

            LayoutImportExport::importLayout(doc.object(), m_view->rootState());

            // move the new element to its position and set a sane initial size
            ModifyElementCommand poscmd(element);
            QPointF pos = m_pos;
            QSizeF size = element->preferredSize();
            if (size.width() > 0)
                pos.setX(qMax<qreal>(0, pos.x() - size.width()/2));
            if (size.height() > 0)
                pos.setY(qMax<qreal>(0, pos.y() - size.height()/2));
            poscmd.setGeometry(QRectF(pos, size));
            poscmd.redo();

            // Mark the new Element as current one what means the item is selected
            // as if a user clicked on it.
            m_view->setCurrentItem(element);
        }
        void undo() override {
            m_createcmd->undo();
        }
    private:
        StateMachineScene *m_view;
        QScopedPointer<CreateElementCommand> m_createcmd;
        QPointF m_pos;
    };

    // TODO: Try to decouple more
    auto view = stateMachineView()->scene();
    CreateAndPositionCommand *cmd = new CreateAndPositionCommand(view, type, target, QPointF(pos));
    stateMachineView()->sendCommand(cmd);

    return true;
}
