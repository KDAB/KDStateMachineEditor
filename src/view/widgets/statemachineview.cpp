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
#include "command/deleteelementcommand.h"
#include "command/changestatemachinecommand.h"
#include "command/modifylayoutitemcommand.h"
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
#include "layoutimportexport.h"
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
    Q_FOREACH (const QQmlError &e, errors) {
        s += e.toString() + '\n';
    }
    return s;
}

struct StateMachineView::Private
{
    Private(StateMachineView* q);

    StateMachineView* q;

    View* m_view;

    CommandController* m_controller;
    ConfigurationController* m_configurationController;
    EditController* m_editController;

    bool m_editModeEnabled;

    QRectF adjustedViewRect();

    // slots
    void onRootLayoutItemChanged(KDSME::LayoutItem* root);
    void onStateMachineChanged(KDSME::StateMachine* stateMachine);
};

StateMachineView::Private::Private(StateMachineView* q)
    : q(q)
    , m_view(nullptr)
    , m_controller(nullptr)
    , m_configurationController(nullptr)
    , m_editController(nullptr)
    , m_editModeEnabled(false)
{
}

StateMachineView::StateMachineView(QWidget* parent)
    : QQuickWidget(parent)
    , d(new Private(this))
{
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
    qmlRegisterUncreatableType<HistoryState>(KDSME_QML_NAMESPACE, 1, 0, "HistoryState", "Access to object");
    qmlRegisterUncreatableType<PseudoState>(KDSME_QML_NAMESPACE, 1, 0, "PseudoState", "Access to object");
    qmlRegisterUncreatableType<Transition>(KDSME_QML_NAMESPACE, 1, 0, "Transition", "Access to object");

    // singletons
    qmlRegisterSingletonType<QuickKDSMEGlobal>(KDSME_QML_NAMESPACE, 1, 0, "Global", kdsme_global_singletontype_provider);
    qmlRegisterSingletonType<CommandFactory>(KDSME_QML_NAMESPACE, 1, 0, "CommandFactory", kdsme_commandFactory_singletontype_provider);

    d->m_controller = new CommandController(new QUndoStack(this), this);
    d->m_configurationController = new ConfigurationController(this);
    d->m_editController = new EditController(this);

    engine()->rootContext()->setContextProperty("_quickView", this);

    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSource(QUrl("qrc:/kdsme/qml/StateMachineView.qml"));
    Q_ASSERT_X(errors().isEmpty(), __FUNCTION__, qPrintable(kdsme_qmlErrorString(errors())));
}

StateMachineView::~StateMachineView()
{
}


View* StateMachineView::view() const
{
    return d->m_view;
}

void StateMachineView::setView(View* view)
{
    if (d->m_view == view)
        return;

    if (d->m_view) {
        disconnect(d->m_view, 0, this, 0);
    }
    d->m_view = view;
    if (d->m_view) {
        connect(d->m_view, SIGNAL(rootLayoutItemChanged(KDSME::LayoutItem*)), this, SLOT(onRootLayoutItemChanged(KDSME::LayoutItem*)));
        connect(d->m_view, SIGNAL(stateMachineChanged(KDSME::StateMachine*)), this, SLOT(onStateMachineChanged(KDSME::StateMachine*)));
    }

    emit viewChanged(d->m_view);
}

CommandController* StateMachineView::commandController() const
{
    return d->m_controller;
}

ConfigurationController* StateMachineView::configurationController() const
{
    return d->m_configurationController;
}

EditController* StateMachineView::editController() const
{
    return d->m_editController;
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

void StateMachineView::Private::onRootLayoutItemChanged(KDSME::LayoutItem* root)
{
    Q_UNUSED(root);
}

void StateMachineView::Private::onStateMachineChanged(KDSME::StateMachine* stateMachine)
{
    Q_UNUSED(stateMachine);
}

void StateMachineView::changeStateMachine(KDSME::StateMachine *stateMachine)
{
    Q_ASSERT(d->m_view);
    ChangeStateMachineCommand* cmd = new ChangeStateMachineCommand(d->m_view);
    cmd->setStateMachine(stateMachine);
    if (d->m_view->stateMachine()) {
        commandController()->push(cmd);
    } else {
        cmd->redo();
        delete cmd;
    }
}

void StateMachineView::deleteElement(KDSME::Element *element)
{
    DeleteElementCommand* cmd = new DeleteElementCommand(d->m_view, element);
    commandController()->push(cmd);
}

qreal StateMachineView::zoom() const
{
    QQuickItem* scene = sceneObject();
    return scene->scale();
}

void StateMachineView::setZoom(qreal value)
{
    QQuickItem* scene = sceneObject();
    if (qFuzzyCompare(scene->scale(), value))
        return;

    scene->setTransformOrigin(QQuickItem::TopLeft);
    scene->setScale(value);
}

QRectF StateMachineView::Private::adjustedViewRect()
{
    static const int margin = 10;

    const QQuickItem* viewPort = q->rootObject()->findChild<QQuickItem*>("scrollView");
    const QRectF viewRect(viewPort->x(), viewPort->y(), viewPort->width(), viewPort->height());
    if (viewRect.isEmpty())
        return QRectF();

    return viewRect.adjusted(margin, margin, -margin, -margin);
}

void StateMachineView::fitInView(const QRectF& rect)
{
    const QQuickItem* viewPort = viewPortObject();

    QRectF sceneRect = rect;
    if (rect.isEmpty()) {
        const qreal width = viewPort->property("contentWidth").toReal();
        const qreal height = viewPort->property("contentHeight").toReal();
        sceneRect = QRectF(0, 0, width, height);
    }

    QRectF viewRect = d->adjustedViewRect();

    if (sceneRect.isEmpty() || viewRect.isEmpty())
        return;

    qreal horizontalScale = viewRect.width() / sceneRect.width();
    qreal verticalScale = viewRect.height() / sceneRect.height();

    QQuickItem* scene = sceneObject();
    const qreal scale = scene->scale() * qMin(horizontalScale, verticalScale);
    scene->setScale(scale);
}

void StateMachineView::sendCommand(Command* cmd)
{
    Q_ASSERT(commandController());
    commandController()->undoStack()->push(cmd);
}

#include "moc_statemachineview.cpp"
