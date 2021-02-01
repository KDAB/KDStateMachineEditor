/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "graphvizlayouterbackend_p.h"

#include <config-kdsme.h>

#include "gvutils.h"

#include "state.h"
#include "transition.h"
#include "elementmodel.h"
#include "elementwalker.h"
#include "layoutproperties.h"
#include "layoututils.h"
#include "util/objecthelper.h"

#include <graphviz/gvc.h>
#ifdef WITH_CGRAPH
#include <graphviz/cgraph.h>
#else
#include <graphviz/graph.h>
#endif

#include "debug.h"

#include <QDir>
#include <QFile>
#include <QPainterPath>
#include <QPoint>

#include <clocale> //for LC_NUMERIC

#define IF_DEBUG(x)

using namespace KDSME;
using namespace KDSME::GVUtils;
using namespace ObjectHelper;

namespace {

/// Default Graphviz layouting  tool, see https://www.graphviz.org/
const char DEFAULT_LAYOUT_TOOL[] = "dot";

/**
 * Default DPI for dot.
 *
 * Dot uses a 72 DPI value for converting it's position coordinates from points to pixels
 * while we display at 96 DPI on most operating systems.
 */
const qreal DOT_DEFAULT_DPI = 72.0;
const qreal DISPLAY_DPI = 96.0;
const qreal TO_DOT_DPI_RATIO = DISPLAY_DPI / DOT_DEFAULT_DPI;

QVector<QPair<const char*, const char*>> attributesForState(const State* state)
{
    typedef QPair<const char*, const char*> EntryType;
    typedef QVector<EntryType> ResultType;

    if (const PseudoState* pseudoState = qobject_cast<const PseudoState*>(state)) {
        switch (pseudoState->kind()) {
        case PseudoState::InitialState:
            return ResultType()
                << EntryType("label", "") // get rid off 'no space for label' warnings
                << EntryType("shape", "circle")
                << EntryType("fixedsize", "true")
                << EntryType("height", "0.20")
                << EntryType("width", "0.20");
        default:
            break;
        }
    }

    if (state->type() == State::HistoryStateType) {
        return ResultType()
                << EntryType("label", "H*") // get rid off 'no space for label' warnings
                << EntryType("shape", "circle")
                << EntryType("fixedsize", "true");
    }

    if (state->type() == State::FinalStateType) {
        return ResultType()
            << EntryType("shape", "doublecircle")
            << EntryType("label", "")
            << EntryType("style", "filled")
            << EntryType("fillcolor", "black")
            << EntryType("fixedsize", "true")
            << EntryType("height", "0.15")
            << EntryType("width", "0.15");
    }

    // default:
    return ResultType()
        << EntryType("shape", "rectangle")
        << EntryType("style", "rounded");
  }
}

struct GraphvizLayouterBackend::Private
{
    Private();
    ~Private();

    void buildState(State* state, Agraph_t* graph);
    void buildTransitions(State* state, Agraph_t* graph);
    void buildTransition(Transition* transition, Agraph_t* graph);

    void import();
    void importItem(Element* item, void* obj);
    void importState(State* state, Agnode_t* node);
    void importState(State* state, Agraph_t* graph);
    void importTransition(Transition* transition, Agedge_t* edge);

    /// Allocate resources from Graphviz
    void openContext(const QString& id);
    /// Free resources
    void closeLayout();

    /**
     * Return the bounding rect of the label for graph @p graph in global coordinate system
     */
    QRectF boundingRectForGraph(Agraph_t *graph) const;
    /**
     * Return the bounding rect of the label for edge @p edge in global coordinate system
     */
    QRectF labelRectForEdge(Agedge_t* edge) const;
    /**
     * Return the painter path for edge @p edge in global coordinate system
     */
    QPainterPath pathForEdge(Agedge_t* edge) const;

    inline Agnode_t* agnodeForState(State* state);

    /// Root Graphviz graph used for layouting
    Agraph_t* m_graph;
    /// Graphviz context
    GVC_t* m_context;

    LayoutMode m_layoutMode;
    const LayoutProperties* m_properties;

    /// Mapping from state machine items to Graphviz layout items
    QPointer<State> m_root;
    QHash<Element*, Agnode_t*> m_elementToDummyNodeMap;
    QHash<Element*, void*> m_elementToPointerMap;
};

GraphvizLayouterBackend::Private::Private()
    : m_graph(nullptr)
    , m_context(nullptr)
    , m_layoutMode(RecursiveMode)
    , m_properties(0)
{
    // hide non-critical warnings, such as
    // Warning: node '0x15e1800', graph 'GraphvizLayouterBackend@0xa90330' size too small for label
    // decrease debug level to AGERR from AGWARN (default)
    agseterr(AGERR);
}

GraphvizLayouterBackend::Private::~Private()
{
}

bool isAncestorCollapsed(State* state)
{
    auto current = state;
    while (current) {
        auto parent = current->parentState();
        if (parent && !parent->isExpanded())
            return true;

        current = parent;
    }
    return false;
}

void GraphvizLayouterBackend::Private::buildState(State* state, Agraph_t* graph)
{
    Q_ASSERT(state);
    Q_ASSERT(graph);

    IF_DEBUG(qCDebug(KDSME_CORE) << state->label() << *state << graph);

    LocaleLocker lock;

    // build nodes
    if (m_layoutMode == RecursiveMode && !state->childStates().isEmpty()) {
        const QString graphName = "cluster" + addressToString(state);
        Agraph_t* newGraph = _agsubg(graph, graphName);

        m_elementToPointerMap[state] = newGraph;
        _agset(newGraph, "label", state->label().isEmpty() ? QObject::tr("<unnamed>") : state->label() + " ###"); // add a placeholder for the expand/collapse button

        auto dummyNode = _agnode(newGraph,  "dummynode_" + graphName);
        _agset(dummyNode, "shape", "point");
        _agset(dummyNode, "style", "invis");
        m_elementToDummyNodeMap[state] = dummyNode;

        if (!isAncestorCollapsed(state)) {
            Q_FOREACH (State* childState, state->childStates()) {
                buildState(childState, newGraph);
            }
        }
    } else {
        if (m_layoutMode == RecursiveMode && isAncestorCollapsed(state)) {
            return;
        }

        Agnode_t* newNode = _agnode(graph, addressToString(state));
        m_elementToPointerMap[state] = newNode;

        if (!qIsNull(state->width()) && !qIsNull(state->height())) {
            _agset(newNode, "width", QString::number(state->width() / DISPLAY_DPI));
            _agset(newNode, "height", QString::number(state->height() / DISPLAY_DPI));
            _agset(newNode, "fixedsize", "true");
        }
        if (!state->label().isEmpty()) {
            _agset(newNode, "label", state->label());
        }

        foreach (const auto& kv, attributesForState(qobject_cast<State*>(state))) {
            _agset(newNode, kv.first, kv.second);
        }
    }
}

void GraphvizLayouterBackend::Private::buildTransitions(State* state, Agraph_t* graph)
{
    IF_DEBUG(qCDebug(KDSME_CORE) << state->label() << *state << graph);

    foreach (Transition* transition, state->transitions()) {
        buildTransition(transition, graph);
    }

    if (m_layoutMode == RecursiveMode) {
        foreach (State* childState, state->childStates()) {
            buildTransitions(childState, graph); // recursive call
        }
    }
}

void GraphvizLayouterBackend::Private::buildTransition(Transition* transition, Agraph_t* graph)
{
    if (!transition->targetState()) {
        return;
    }

    if (m_layoutMode == RecursiveMode && isAncestorCollapsed(transition->sourceState())) {
        return;
    }

    IF_DEBUG(qCDebug(KDSME_CORE) << transition->label() << *transition << graph);

    const auto sourceState = transition->sourceState();
    const auto targetState = transition->targetState();

    Agnode_t* source = agnodeForState(sourceState);
    if (!source) return;
    Agnode_t* target = agnodeForState(targetState);
    if (!target) return;

    auto sourceDummyNode = m_elementToDummyNodeMap.value(sourceState);
    auto targetDummyNode = m_elementToDummyNodeMap.value(targetState);

    Agedge_t* edge = _agedge(graph,
                             sourceDummyNode ? sourceDummyNode : source,
                             targetDummyNode ? targetDummyNode : target,
                             addressToString(transition), true);
    if (!transition->label().isEmpty() && m_properties->showTransitionLabels()) {
        _agset(edge, "label", transition->label());
    }

    // in order to connect subgraphs we need to leverage ltail + lhead attribute of edges
    // see: https://stackoverflow.com/questions/2012036/graphviz-how-to-connect-subgraphs
    if (sourceDummyNode) {
        const QString graphName = "cluster" + addressToString(sourceState);
        _agset(edge, "ltail", graphName);
    }
    if (targetDummyNode) {
        const QString graphName = "cluster" + addressToString(targetState);
        _agset(edge, "lhead", graphName);
    }
    m_elementToPointerMap[transition] = edge;
    Q_ASSERT(edge);
}

void GraphvizLayouterBackend::Private::import()
{
    IF_DEBUG(qCDebug(KDSME_CORE) << m_elementToPointerMap.keys();)

    LocaleLocker lock;
    ElementWalker walker(ElementWalker::PreOrderTraversal);
    walker.walkItems(m_root, [this](Element* element) {
        if (auto obj = m_elementToPointerMap.value(element)) {
            importItem(element, obj);
        }
        return ElementWalker::RecursiveWalk;
    });
}

void GraphvizLayouterBackend::Private::importItem(Element* item, void* obj)
{
    if (State* state = qobject_cast<State*>(item)) {
        if (m_layoutMode == RecursiveMode && !state->childStates().isEmpty()) {
            importState(state, static_cast<Agraph_t*>(obj));
        } else {
            importState(state, static_cast<Agnode_t*>(obj));
        }
    } else if (Transition* transition = qobject_cast<Transition*>(item)) {
        importTransition(transition, static_cast<Agedge_t*>(obj));
    }
}

void GraphvizLayouterBackend::Private::importState(State* state, Agnode_t* node)
{
    Q_ASSERT(state);
    Q_ASSERT(node);

    IF_DEBUG(qCDebug(KDSME_CORE) << "before" << state->label() << *state << node);

    // Fetch the X coordinate, apply the DPI conversion rate (actual DPI / 72, used by dot)
    const qreal x = ND_coord(node).x * TO_DOT_DPI_RATIO;

    // Translate the Y coordinate from bottom-left to top-left corner
    const qreal y = (GD_bb(m_graph).UR.y - ND_coord(node).y) * TO_DOT_DPI_RATIO;

    // Transform the width and height from inches to pixels

    state->setWidth(ND_width(node) * DISPLAY_DPI);
    state->setHeight(ND_height(node) * DISPLAY_DPI);

    const QPointF absolutePos = QPointF(x - state->width()/2, y - state->height()/2);
    if (m_layoutMode == RecursiveMode) {
        const QPointF relativePos = absolutePos - (state->parentElement() ? state->parentElement()->absolutePos() : QPointF());
        state->setPos(relativePos);
    } else {
        state->setPos(absolutePos);
    }

    IF_DEBUG(qCDebug(KDSME_CORE) << "after" << state->label() << *state << node);
}

void GraphvizLayouterBackend::Private::importState(State* state, Agraph_t* graph)
{
    Q_ASSERT(state);
    Q_ASSERT(graph);

    IF_DEBUG(qCDebug(KDSME_CORE) << "before" << state->label() << *state << graph);

    QRectF rect = boundingRectForGraph(graph);

    state->setWidth(rect.width());
    state->setHeight(rect.height());

    const QPointF absolutePos = rect.topLeft();
    if (m_layoutMode == RecursiveMode) {
        const QPointF relativePos = absolutePos - (state->parentElement() ? state->parentElement()->absolutePos() : QPointF());
        state->setPos(relativePos);
    } else {
        state->setPos(absolutePos);
    }

    IF_DEBUG(qCDebug(KDSME_CORE) << "after" << state->label() << *state << graph);
}

void GraphvizLayouterBackend::Private::importTransition(Transition* transition, Agedge_t* edge)
{
    Q_ASSERT(transition);
    Q_ASSERT(edge);

    IF_DEBUG(qCDebug(KDSME_CORE) << "before" << transition << edge);
    // transform to local coordinate system, set position offset
    const QPainterPath path = pathForEdge(edge);
    const QRectF labelRect = labelRectForEdge(edge);
    const QRectF boundingRect = labelRect.united(path.boundingRect());
    const QPointF absolutePos = boundingRect.topLeft();
    Q_ASSERT(transition->parentElement());
    if (m_layoutMode == RecursiveMode) {
        const QPointF relativePos = absolutePos - transition->parentElement()->absolutePos();
        transition->setPos(relativePos);
    } else {
        transition->setPos(absolutePos - transition->parentElement()->pos());
    }
    transition->setShape(path.translated(-absolutePos));
    transition->setLabelBoundingRect(labelRect.translated(-absolutePos));
    IF_DEBUG(qCDebug(KDSME_CORE) << "after" << transition << edge);
}

#if WITH_STATIC_GRAPHVIZ
extern "C" GVC_t* gvContextWithStaticPlugins();
#endif

void GraphvizLayouterBackend::Private::openContext(const QString& id)
{
    LocaleLocker lock;

    m_elementToDummyNodeMap.clear();
    m_elementToPointerMap.clear();

#ifdef WITH_CGRAPH
    m_graph = _agopen(id, Agdirected, &AgDefaultDisc);
#else
    m_graph = _agopen(id, AGDIGRAPH);
#endif

    // modify settings
    if (m_layoutMode == RecursiveMode) {
        _agset(m_graph, "compound", "true");
    }
    _agset(m_graph, "overlap", "prism");
    _agset(m_graph, "overlap_shrink", "true");
    _agset(m_graph, "splines", "true");
    _agset(m_graph, "pad", "0.0");
    _agset(m_graph, "dpi", "96.0");
    _agset(m_graph, "nodesep", "0.2");
}

void GraphvizLayouterBackend::Private::closeLayout()
{
    if (!m_graph) {
        return;
    }

    // TODO: Intentional leak: Graphviz segfaults otherwise
    //gvFreeLayout(m_context, m_graph);

    agclose(m_graph);
    m_graph = nullptr;

    m_root = nullptr;
    m_properties = nullptr;

    agreseterrors();
}

QRectF GraphvizLayouterBackend::Private::boundingRectForGraph(Agraph_t *graph) const
{
    const qreal left = GD_bb(graph).LL.x * TO_DOT_DPI_RATIO;
    const qreal top = (GD_bb(m_graph).UR.y - GD_bb(graph).LL.y) * TO_DOT_DPI_RATIO;
    const qreal right = GD_bb(graph).UR.x * TO_DOT_DPI_RATIO;
    const qreal bottom = (GD_bb(m_graph).UR.y - GD_bb(graph).UR.y) * TO_DOT_DPI_RATIO;
    return QRectF(left, top, right - left, bottom - top).normalized();
}

QRectF GraphvizLayouterBackend::Private::labelRectForEdge(Agedge_t* edge) const
{
    if (!ED_label(edge))
        return QRectF();

    // note that the position attributes in graphviz point to the *center* of this element.
    // we need to subtract half of the width/height to get the top-left position
    const double posx = ED_label(edge)->pos.x;
    const double posy = ED_label(edge)->pos.y;
    const QRectF labelBoundingRect = QRectF(
        (posx - ED_label(edge)->dimen.x / 2.0) * TO_DOT_DPI_RATIO,
        ((GD_bb(m_graph).UR.y - posy) - ED_label(edge)->dimen.y / 2.0) * TO_DOT_DPI_RATIO,
        ED_label(edge)->dimen.x * TO_DOT_DPI_RATIO,
        ED_label(edge)->dimen.y * TO_DOT_DPI_RATIO);
    return labelBoundingRect;
}

QPainterPath GraphvizLayouterBackend::Private::pathForEdge(Agedge_t* edge) const
{
    QPainterPath path;
    //Calculate the path from the spline (only one spline, as the graph is strict.
    //If it wasn't, we would have to iterate over the first list too)
    //Calculate the path from the spline (only one as the graph is strict)
    if (ED_spl(edge) && (ED_spl(edge)->list != nullptr) && (ED_spl(edge)->list->size%3 == 1)) {
        //If there is a starting point, draw a line from it to the first curve point
        if (ED_spl(edge)->list->sflag) {
            path.moveTo(ED_spl(edge)->list->sp.x * TO_DOT_DPI_RATIO,
                (GD_bb(m_graph).UR.y - ED_spl(edge)->list->sp.y) * TO_DOT_DPI_RATIO);
            path.lineTo(ED_spl(edge)->list->list[0].x * TO_DOT_DPI_RATIO,
                (GD_bb(m_graph).UR.y - ED_spl(edge)->list->list[0].y) * TO_DOT_DPI_RATIO);
        } else {
            path.moveTo(ED_spl(edge)->list->list[0].x * TO_DOT_DPI_RATIO,
                (GD_bb(m_graph).UR.y - ED_spl(edge)->list->list[0].y) * TO_DOT_DPI_RATIO);
        }

        //Loop over the curve points
        for (int i=1; i<ED_spl(edge)->list->size; i+=3) {
            path.cubicTo(ED_spl(edge)->list->list[i].x * TO_DOT_DPI_RATIO,
                (GD_bb(m_graph).UR.y - ED_spl(edge)->list->list[i].y) * TO_DOT_DPI_RATIO,
                ED_spl(edge)->list->list[i+1].x * TO_DOT_DPI_RATIO,
                (GD_bb(m_graph).UR.y - ED_spl(edge)->list->list[i+1].y) * TO_DOT_DPI_RATIO,
                ED_spl(edge)->list->list[i+2].x * TO_DOT_DPI_RATIO,
                (GD_bb(m_graph).UR.y - ED_spl(edge)->list->list[i+2].y) * TO_DOT_DPI_RATIO);
        }

        //If there is an ending point, draw a line to it
        if(ED_spl(edge)->list->eflag) {
            path.lineTo(ED_spl(edge)->list->ep.x * TO_DOT_DPI_RATIO,
                (GD_bb(m_graph).UR.y - ED_spl(edge)->list->ep.y) * TO_DOT_DPI_RATIO);
        }
    }
    return path;
}

Agnode_t* GraphvizLayouterBackend::Private::agnodeForState(State* state)
{
    Q_ASSERT(state);
    return static_cast<Agnode_t*>(m_elementToPointerMap.value(state));
}

GraphvizLayouterBackend::GraphvizLayouterBackend()
    : d(new Private)
{
    // create context
#if WITH_STATIC_GRAPHVIZ
    d->m_context = gvContextWithStaticPlugins();
#else
    d->m_context = gvContext();
#endif
    Q_ASSERT(d->m_context);
}

GraphvizLayouterBackend::~GraphvizLayouterBackend()
{
    closeLayout();

    // close context
    Q_ASSERT(d->m_context);
    gvFreeContext(d->m_context);
    d->m_context = nullptr;

    delete d;
}

GraphvizLayouterBackend::LayoutMode GraphvizLayouterBackend::layoutMode() const
{
    return d->m_layoutMode;
}

void GraphvizLayouterBackend::setLayoutMode(LayoutMode mode)
{
    d->m_layoutMode = mode;
}

void GraphvizLayouterBackend::layout()
{
    // do the actual layouting
    _gvLayout(d->m_context, d->m_graph, DEFAULT_LAYOUT_TOOL);

    if (qEnvironmentVariableIsSet("KDSME_DEBUG_GRAPHVIZ")) {
        const auto state = d->m_root;
        const auto machine = state->machine();
        Q_ASSERT(machine);
        const QString machineName = !machine->label().isEmpty() ?
            machine->label() :
            ObjectHelper::addressToString(machine);
        const QString stateName = !state->label().isEmpty() ?
            state->label() :
            ObjectHelper::addressToString(state);
        QDir tmpDir = QDir::temp();
        tmpDir.mkdir("kdsme_debug");
        const QString baseName = QString("%1/%2_%3").arg(tmpDir.filePath("kdsme_debug")).arg(machineName).arg(stateName);
        saveToFile(baseName + ".png");
        saveToFile(baseName + ".dot", "dot");
    }
}

void GraphvizLayouterBackend::saveToFile(const QString& filePath, const QString& format)
{
    if (!d->m_context) {
        qCDebug(KDSME_CORE) << "Cannot render image, context not open:" << filePath;
        return;
    }

    LocaleLocker lock;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        const int rc = gvRenderFilename(d->m_context, d->m_graph, qPrintable(format), qPrintable(filePath));
        if (rc != 0) {
            qCDebug(KDSME_CORE) << "gvRenderFilename to" << filePath << "failed with return-code:" <<  rc;
        }
    } else {
        qCDebug(KDSME_CORE) << "Cannot render image, cannot open:" << filePath;
        return;
    }
}

void GraphvizLayouterBackend::openLayout(State* state, const LayoutProperties* properties)
{
    d->m_root = state;
    d->m_properties = properties;

    d->openContext(QString("GraphvizLayouterBackend@%1").arg(addressToString(this)));
}

void GraphvizLayouterBackend::closeLayout()
{
    d->closeLayout();
}

void GraphvizLayouterBackend::buildState(State* state)
{
    d->buildState(state, d->m_graph);
}

void GraphvizLayouterBackend::buildTransitions(State* state)
{
    d->buildTransitions(state, d->m_graph);
}

void GraphvizLayouterBackend::buildTransition(Transition* transition)
{
    d->buildTransition(transition, d->m_graph);
}

void GraphvizLayouterBackend::import()
{
    d->import();
}

QRectF GraphvizLayouterBackend::boundingRect() const
{
    if (!d->m_graph)
        return QRectF();

    return d->boundingRectForGraph(d->m_graph);
}
