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

#include "statemachineview.h"

#include "commandcontroller.h"
#include "command/command_p.h"
#include "command/commandfactory_p.h"
#include "command/createelementcommand_p.h"
#include "command/deleteelementcommand_p.h"
#include "command/changestatemachinecommand_p.h"
#include "command/modifyelementcommand_p.h"
#include "common/metatypedeclarations.h"
#include "debug.h"
#include "editcontroller.h"
#include "layoutimportexport.h"
#include "quick/quickpainterpath_p.h"
#include "quick/quickpen_p.h"
#include "quick/quickprimitiveitem_p.h"
#include "quick/quickmaskedmousearea_p.h"
#include "quick/quickkdsmeglobal_p.h"
#include "quick/quickrecursiveinstantiator_p.h"
#include "quick/quicksceneitem_p.h"
#include "state.h"
#include "transition.h"
#include "elementmodel.h"
#include "layoutproperties.h"
#include "semanticzoommanager.h"
#include "kdsmeconstants.h"
#include "depthchecker.h"
#include "runtimecontroller.h"
#include "statemachinescene.h"

#include <QDir>
#include <QFileInfo>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>

using namespace KDSME;

namespace {

QObject *kdsme_global_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new QuickKDSMEGlobal;
}

QObject *kdsme_commandFactory_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new CommandFactory;
}

#if !defined(NDEBUG)
QString kdsme_qmlErrorString(const QList<QQmlError> &errors)
{
    QString s;
    for (const QQmlError &e : errors) {
        s += e.toString() + u'\n';
    }
    return s;
}
#endif

} // namespace

struct StateMachineView::Private // NOLINT(clang-analyzer-cplusplus.NewDelete)
{
    Private(StateMachineView *q);

    StateMachineView *q;

    QPointer<StateMachineScene> m_scene; // potentially deleted by the QML engine, thus QPointer

    CommandController *m_controller;
    EditController *m_editController;
    QString m_themeName;
    bool m_editModeEnabled;

    [[nodiscard]] QRectF adjustedViewRect() const;

    void onStateMachineChanged(KDSME::StateMachine *stateMachine);
};

StateMachineView::Private::Private(StateMachineView *q)
    : q(q)
    , m_scene(nullptr)
    , m_controller(nullptr)
    , m_editController(nullptr)
    , m_themeName(q->defaultThemeName())
    , m_editModeEnabled(false)
{
}

StateMachineView::StateMachineView(QWidget *parent)
    : QQuickWidget(parent)
    , d(new Private(this))
{
    qRegisterMetaType<QPainterPath>();
    qRegisterMetaType<Qt::PenStyle>();
    qRegisterMetaType<CommandController *>();

    qRegisterMetaType<LayoutProperties *>();
    qRegisterMetaType<State *>();
    qRegisterMetaType<Transition *>();
    qRegisterMetaType<Element *>();
    qRegisterMetaType<Element::Type>();
    qRegisterMetaType<State *>();
    qRegisterMetaType<StateMachine *>();
    qRegisterMetaType<StateModel *>();
    qRegisterMetaType<ObjectTreeModel *>();
    qRegisterMetaType<AbstractScene::ViewState>();
    qRegisterMetaType<GLenum>();
    qRegisterMetaType<ObjectTreeModel *>();

    // creatable types
    qmlRegisterType<DepthChecker>(KDSME_QML_NAMESPACE, 1, 0, "DepthChecker");
    qmlRegisterType<QuickMaskedMouseArea>(KDSME_QML_NAMESPACE, 1, 0, "MaskedMouseArea");
    qmlRegisterType<QuickPainterPath>(KDSME_QML_NAMESPACE, 1, 0, "PainterPath");
    qmlRegisterType<QuickPainterPathGeometryItem>(KDSME_QML_NAMESPACE, 1, 0, "PainterPathGeometry");
    qmlRegisterType<QuickPainterPathStroker>(KDSME_QML_NAMESPACE, 1, 0, "PainterPathStroker");
    qmlRegisterType<QuickPen>(KDSME_QML_NAMESPACE, 1, 0, "Pen");
    qmlRegisterType<QuickGeometryItem>(KDSME_QML_NAMESPACE, 1, 0, "Geometry");
    qmlRegisterType<QuickPrimitiveItem>(KDSME_QML_NAMESPACE, 1, 0, "Primitive");
    qmlRegisterType<QuickRecursiveInstantiator>(KDSME_QML_NAMESPACE, 1, 0, "RecursiveInstantiator");
    qmlRegisterType<QuickSceneItem>(KDSME_QML_NAMESPACE, 1, 0, "SceneItem");
    qmlRegisterType<QuickStateItem>(KDSME_QML_NAMESPACE, 1, 0, "StateItem");
    qmlRegisterType<QuickTransitionItem>(KDSME_QML_NAMESPACE, 1, 0, "TransitionItem");
    qmlRegisterType<PainterPathMask>(KDSME_QML_NAMESPACE, 1, 0, "PainterPathMask");
    qmlRegisterType<SemanticZoomManager>(KDSME_QML_NAMESPACE, 1, 0, "SemanticZoomManager");
    qmlRegisterType<StateMachineScene>(KDSME_QML_NAMESPACE, 1, 0, "StateMachineScene");

    qmlRegisterUncreatableType<AbstractMask>(KDSME_QML_NAMESPACE, 1, 0, "AbstractMask", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<AbstractScene>(KDSME_QML_NAMESPACE, 1, 0, "AbstractScene", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<EditController>(KDSME_QML_NAMESPACE, 1, 0, "EditController", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<CommandController>(KDSME_QML_NAMESPACE, 1, 0, "CommandController", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<RuntimeController>(KDSME_QML_NAMESPACE, 1, 0, "RuntimeController", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<Element>(KDSME_QML_NAMESPACE, 1, 0, "Element", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<HistoryState>(KDSME_QML_NAMESPACE, 1, 0, "HistoryState", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<PseudoState>(KDSME_QML_NAMESPACE, 1, 0, "PseudoState", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<State>(KDSME_QML_NAMESPACE, 1, 0, "State", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<Transition>(KDSME_QML_NAMESPACE, 1, 0, "Transition", QStringLiteral("Access to object"));

    // singletons
    qmlRegisterSingletonType<QuickKDSMEGlobal>(KDSME_QML_NAMESPACE, 1, 0, "Global", kdsme_global_singletontype_provider);
    qmlRegisterSingletonType<CommandFactory>(KDSME_QML_NAMESPACE, 1, 0, "CommandFactory", kdsme_commandFactory_singletontype_provider);

    d->m_controller = new CommandController(new QUndoStack(this), this);
    d->m_editController = new EditController(this);

    engine()->rootContext()->setContextProperty(QStringLiteral("_quickView"), this);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Does not seem to work under Qt 6 -- yields a black QML scene if enabled there
    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);
#endif

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSource(QUrl(QStringLiteral("qrc:/kdsme/qml/StateMachineView.qml")));
#if !defined(NDEBUG)
    Q_ASSERT_X(errors().isEmpty(), __FUNCTION__, qPrintable(kdsme_qmlErrorString(errors())));
#endif
}

StateMachineView::~StateMachineView()
{
}


StateMachineScene *StateMachineView::scene() const
{
    return d->m_scene;
}

void StateMachineView::setScene(StateMachineScene *scene)
{
    if (d->m_scene == scene)
        return;

    if (d->m_scene) {
        disconnect(d->m_scene, nullptr, this, nullptr);
    }
    d->m_scene = scene;
    if (d->m_scene) {
        // clang-format off
        connect(d->m_scene, SIGNAL(stateMachineChanged(KDSME::StateMachine*)), // clazy:exclude=old-style-connect
                this, SLOT(onStateMachineChanged(KDSME::StateMachine*)));
        // clang-format on
    }

    Q_EMIT sceneChanged(d->m_scene);
}

CommandController *StateMachineView::commandController() const
{
    return d->m_controller;
}

EditController *StateMachineView::editController() const
{
    return d->m_editController;
}

QString StateMachineView::defaultThemeName() const
{
    return QStringLiteral("DarkTheme");
}

QString StateMachineView::themeName() const
{
    return d->m_themeName;
}

void StateMachineView::setThemeName(const QString &themeName)
{
    if (d->m_themeName == themeName)
        return;

    QString selectedThemeName = themeName;
    if (themeName.isEmpty()) {
        selectedThemeName = defaultThemeName();
    } else {
        const QString themeFile = QStringLiteral(":/kdsme/qml/themes/%1.qml").arg(themeName);
        if (!QFileInfo::exists(themeFile)) {
            qCWarning(KDSME_VIEW) << "Theme file" << themeFile << "does not exist, using fallback";
            selectedThemeName = defaultThemeName();
        }
    }

    d->m_themeName = std::move(selectedThemeName);
    Q_EMIT themeNameChanged(d->m_themeName);
}

QQuickItem *StateMachineView::viewPortObject() const
{
    auto *item = rootObject()->findChild<QQuickItem *>(QStringLiteral("stateMachineViewport"));
    Q_ASSERT(item);
    return item;
}

QQuickItem *StateMachineView::sceneObject() const
{
    auto *item = rootObject()->findChild<QQuickItem *>(QStringLiteral("stateMachineScene"));
    Q_ASSERT(item);
    return item;
}

void StateMachineView::Private::onStateMachineChanged(KDSME::StateMachine *stateMachine)
{
    Q_UNUSED(stateMachine);
}

void StateMachineView::changeStateMachine(KDSME::StateMachine *stateMachine)
{
    Q_ASSERT(d->m_scene);
    auto *cmd = new ChangeStateMachineCommand(d->m_scene);
    cmd->setStateMachine(stateMachine);
    if (d->m_scene->rootState()) {
        commandController()->push(cmd);
    } else {
        cmd->redo();
        delete cmd;
    }
}

void StateMachineView::deleteElement(KDSME::Element *element)
{
    auto *cmd = new DeleteElementCommand(d->m_scene, element);
    commandController()->push(cmd);
}

QRectF StateMachineView::Private::adjustedViewRect() const
{
    static const int margin = 10;

    const QQuickItem *viewPort = q->rootObject()->findChild<QQuickItem *>(QStringLiteral("scrollView"));
    const QRectF viewRect(viewPort->x(), viewPort->y(), viewPort->width(), viewPort->height());
    if (viewRect.isEmpty())
        return QRectF();

    return viewRect.adjusted(margin, margin, -margin, -margin);
}

void StateMachineView::fitInView()
{
    const QRectF sceneRect = scene()->rootState()->boundingRect();
    const QRectF viewRect = d->adjustedViewRect();
    if (sceneRect.isEmpty() || viewRect.isEmpty())
        return;

    const qreal horizontalScale = viewRect.width() / sceneRect.width();
    const qreal verticalScale = viewRect.height() / sceneRect.height();
    const qreal uniformScale = qMin(horizontalScale, verticalScale);
    scene()->zoomBy(uniformScale);
}

void StateMachineView::sendCommand(Command *cmd) const
{
    Q_ASSERT(commandController());
    commandController()->undoStack()->push(cmd);
}

#include "moc_statemachineview.cpp"
