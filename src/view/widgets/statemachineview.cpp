/*
  statemachineview.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "statemachineview.h"

#include "configurationcontroller.h"
#include "commandcontroller.h"
#include "command/command.h"
#include "command/commandfactory.h"
#include "command/createelementcommand.h"
#include "command/changestatemachinecommand.h"
#include "common/metatypedeclarations.h"
#include "editcontroller.h"
#include "quick/quickpainterpath.h"
#include "quick/quickpen.h"
#include "quick/quickprimitiveitem.h"
#include "quick/quickmaskedmousearea.h"
#include "quick/quickkdsmeglobal.h"
#include "element.h"
#include "elementmodel.h"
#include "layoutitem.h"
#include "layoutitemmodel.h"
#include "layoutproperties.h"
#include "semanticzoommanager.h"
#include "kdsmeconstants.h"
#include "view/view.h"

#include <QDir>
#include <QFileInfo>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>

using namespace KDSME;

static QObject* kdsme_global_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new QuickKDSMEGlobal;
}

static QObject* kdsme_commandFactory_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new CommandFactory;
}

static QString kdsme_qmlErrorString(const QList<QQmlError> errors)
{
    QString s;
    Q_FOREACH(const QQmlError &e, errors)
        s += e.toString() + "\n";
    return s;
}

StateMachineView::StateMachineView(QWidget* parent)
    : QQuickWidget(parent)
    , m_view(nullptr)
    , m_controller(new CommandController(new QUndoStack(this), this))
    , m_configurationController(new ConfigurationController(this))
    , m_editController(new EditController(this))
    , m_editModeEnabled(false)
{
    setResizeMode(QQuickWidget::SizeRootObjectToView);

    qRegisterMetaType<QPainterPath>();
    qRegisterMetaType<Qt::PenStyle>();
    qRegisterMetaType<CommandController*>();

    qRegisterMetaType<QAbstractItemModel*>();
    qRegisterMetaType<LayoutItem*>("LayoutItem");
    qRegisterMetaType<LayoutProperties*>();
    qRegisterMetaType<StateLayoutItem*>();
    qRegisterMetaType<TransitionLayoutItem*>();
    qRegisterMetaType<Element*>();
    qRegisterMetaType<Element::Type>();
    qRegisterMetaType<State*>();
    qRegisterMetaType<StateMachine*>();
    qRegisterMetaType<AbstractView::ViewState>();
    qRegisterMetaType<GLenum>();

    // creatable types
    qmlRegisterType<QuickMaskedMouseArea>(KDSME_QML_NAMESPACE, 1, 0, "MaskedMouseArea");
    qmlRegisterType<QuickPainterPath>(KDSME_QML_NAMESPACE, 1, 0, "PainterPath");
    qmlRegisterType<QuickPainterPathGeometryItem>(KDSME_QML_NAMESPACE, 1, 0, "PainterPathGeometry");
    qmlRegisterType<QuickPen>(KDSME_QML_NAMESPACE, 1, 0, "Pen");
    qmlRegisterType<QuickGeometryItem>(KDSME_QML_NAMESPACE, 1, 0, "Geometry");
    qmlRegisterType<QuickPrimitiveItem>(KDSME_QML_NAMESPACE, 1, 0, "Primitive");
    qmlRegisterType<LayoutItemModel>(KDSME_QML_NAMESPACE, 1, 0, "LayoutInformationModel");
    qmlRegisterType<PainterPathMask>(KDSME_QML_NAMESPACE, 1, 0, "PainterPathMask");
    qmlRegisterType<SemanticZoomManager>(KDSME_QML_NAMESPACE, 1, 0, "SemanticZoomManager");

    qmlRegisterUncreatableType<AbstractMask>(KDSME_QML_NAMESPACE, 1, 0, "AbstractMask", "Access to object");
    qmlRegisterUncreatableType<AbstractView>(KDSME_QML_NAMESPACE, 1, 0, "AbstractView", "Access to object");
    qmlRegisterUncreatableType<EditController>(KDSME_QML_NAMESPACE, 1, 0, "EditController", "Access to object");
    qmlRegisterUncreatableType<CommandController>(KDSME_QML_NAMESPACE, 1, 0, "CommandController", "Access to object");
    qmlRegisterUncreatableType<ConfigurationController>(KDSME_QML_NAMESPACE, 1, 0, "ConfigurationController", "Access to object");
    qmlRegisterUncreatableType<LayoutItem>(KDSME_QML_NAMESPACE, 1, 0, "LayoutItem", "Access to object");
    qmlRegisterUncreatableType<View>(KDSME_QML_NAMESPACE, 1, 0, "View", "Access to object");
    qmlRegisterUncreatableType<StateModel>(KDSME_QML_NAMESPACE, 1, 0, "StateModel", "Access to object");
    qmlRegisterUncreatableType<Element>(KDSME_QML_NAMESPACE, 1, 0, "Element", "Access to object");
    qmlRegisterUncreatableType<PseudoState>(KDSME_QML_NAMESPACE, 1, 0, "PseudoState", "Access to object");
    qmlRegisterUncreatableType<Transition>(KDSME_QML_NAMESPACE, 1, 0, "Transition", "Access to object");

    // singletons
    qmlRegisterSingletonType<QuickKDSMEGlobal>(KDSME_QML_NAMESPACE, 1, 0, "Global", kdsme_global_singletontype_provider);
    qmlRegisterSingletonType<CommandFactory>(KDSME_QML_NAMESPACE, 1, 0, "CommandFactory", kdsme_commandFactory_singletontype_provider);

    engine()->rootContext()->setContextProperty("_quickView", this);

    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);

    setSource(QUrl("qrc:/kdsme/qml/StateMachineView.qml"));
    Q_ASSERT_X(errors().isEmpty(), __FUNCTION__, qPrintable(kdsme_qmlErrorString(errors())));
}

View* StateMachineView::view() const
{
    return m_view;
}

void StateMachineView::setView(View* view)
{
    if (m_view == view)
        return;

    if (m_view) {
        disconnect(m_view, &View::rootLayoutItemChanged, this, &StateMachineView::handleNewLayout);
        disconnect(m_view, &View::stateMachineChanged, this, &StateMachineView::handleNewStateMachine);
    }
    m_view = view;
    if (m_view) {
        connect(m_view, &View::rootLayoutItemChanged, this, &StateMachineView::handleNewLayout);
        connect(m_view, &View::stateMachineChanged, this, &StateMachineView::handleNewStateMachine);
    }

    m_configurationController->setView(m_view);
    emit viewChanged(m_view);
}

CommandController* StateMachineView::commandController() const
{
    return m_controller;
}

ConfigurationController* StateMachineView::configurationController() const
{
    return m_configurationController;
}

EditController* StateMachineView::editController() const
{
    return m_editController;
}

QQuickItem* StateMachineView::viewPortObject() const
{
    QQuickItem* item = rootObject()->findChild<QQuickItem*>("stateMachineViewport");
    Q_ASSERT(item);
    return item;
}

QQuickItem* StateMachineView::sceneObject() const
{
    QQuickItem* item = rootObject()->findChild<QQuickItem*>("stateMachineScene");
    Q_ASSERT(item);
    return item;
}

void StateMachineView::handleNewLayout()
{
}

void StateMachineView::handleNewStateMachine()
{
}

void StateMachineView::changeStateMachine(KDSME::StateMachine *stateMachine)
{
    Q_ASSERT(m_view);
    ChangeStateMachineCommand* cmd = new ChangeStateMachineCommand(m_view);
    cmd->setStateMachine(stateMachine);
    if (m_view->stateMachine()) {
        commandController()->push(cmd);
    } else {
        cmd->redo();
        delete cmd;
    }
}

bool StateMachineView::sendDragEnterEvent(LayoutItem* sender, const QPoint& pos, const QList<QUrl>& urls)
{
    Q_UNUSED(sender);
    Q_UNUSED(pos);

    if (urls.isEmpty())
        return false;

    // we only accept one item only for now
    const QUrl url = urls.first();
    if (url.scheme() != KDSME_QML_URI_PREFIX)
        return false;

    return true;
}

bool StateMachineView::sendDropEvent(LayoutItem* sender, const QPoint& pos, const QList<QUrl>& urls)
{
    Q_UNUSED(sender);
    Q_UNUSED(pos);

    if (urls.isEmpty())
        return false;

    // we only accept one item only for now
    const QUrl url = urls.first();
    if (url.scheme() != KDSME_QML_URI_PREFIX)
        return false;

    const QString str = url.toString(QUrl::RemoveScheme);
    const QString typeString = str.split('/').last();
    if (typeString.isEmpty())
        return false;

    Element::Type type = Element::stringToType(qPrintable(typeString));
    auto cmd = new CreateElementCommand(m_view->stateModel(), type);
    commandController()->push(cmd);

    // TODO: Send command to move the newly created LayoutItem to the correct position
    return true;
}

void StateMachineView::fitInView(const QRectF& rect)
{
    static const int margin = 10;

    const QQuickItem* viewPort = viewPortObject();
    const QRectF viewRect(viewPort->x(), viewPort->y(), viewPort->width(), viewPort->height());
    if (rect.isEmpty() || viewRect.isEmpty())
        return;

    QRectF adjustedViewRect = viewRect.adjusted(
        margin, margin, -margin, -margin
    );
    qreal horizontalScale = adjustedViewRect.width() / rect.width();
    qreal verticalScale = adjustedViewRect.height() / rect.height();

    QQuickItem* scene = sceneObject();
    const qreal scale = scene->scale() * qMin(horizontalScale, verticalScale);
    scene->setTransformOrigin(QQuickItem::TopLeft);
    scene->setScale(scale);
}

void StateMachineView::fitInView()
{
    QQuickItem* viewPort = viewPortObject();
    const qreal width = viewPort->property("contentWidth").toReal();
    const qreal height = viewPort->property("contentHeight").toReal();
    const QRectF rect(0, 0, width, height);
    fitInView(rect);
}
