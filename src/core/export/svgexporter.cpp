/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "svgexporter.h"

#include "elementutil.h"
#include "objecthelper.h"
#include "state.h"
#include "transition.h"

#include <QDebug>
#include <QFontMetricsF>
#include <QGuiApplication>
#include <QPainterPath>
#include <QRectF>
#include <QTransform>
#include <QXmlStreamWriter>

using namespace KDSME;

namespace KDSME {

class SvgExporterPrivate
{
public:
    explicit SvgExporterPrivate(SvgExporter *qq)
        : q(qq)
    {
    }

    void writeSvgIntro(const QRectF &viewBox);
    void writeSvgRect(const QRectF &rect, double radius = 0.0);
    void writeSvgEllipse(const QRectF &rect, bool fill);
    void writeSvgLine(const QLineF &line);
    void writeSvgPath(const QPainterPath &path, bool fill = false);
    void writeSvgText(const QString &text, QPointF pos, bool center = false);

    bool writeStateMachine(const StateMachine *machine);
    bool writeState(State *state);
    bool writeStateInner(const State *state);
    bool writeTransition(const Transition *transition);

    [[nodiscard]] static double headerHeight();
    [[nodiscard]] static double margin();
    [[nodiscard]] static double arrowSize();

    SvgExporter *q;
    QXmlStreamWriter writer;
};

}

void SvgExporterPrivate::writeSvgIntro(const QRectF &viewBox)
{
    writer.writeStartElement(QStringLiteral("svg"));
    writer.writeAttribute(QStringLiteral("width"), QString::number(viewBox.width()));
    writer.writeAttribute(QStringLiteral("height"), QString::number(viewBox.height()));
    writer.writeAttribute(QStringLiteral("viewBox"), QStringLiteral("%1 %2 %3 %4").arg(viewBox.x()).arg(viewBox.y()).arg(viewBox.width()).arg(viewBox.height()));
    writer.writeAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.w3.org/2000/svg"));
    writer.writeAttribute(QStringLiteral("xmlns:svg"), QStringLiteral("http://www.w3.org/2000/svg"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("1.2"));
    writer.writeAttribute(QStringLiteral("baseProfile"), QStringLiteral("tiny"));
}

void SvgExporterPrivate::writeSvgRect(const QRectF &rect, double radius)
{
    writer.writeStartElement(QStringLiteral("rect"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(rect.x()));
    writer.writeAttribute(QStringLiteral("y"), QString::number(rect.y()));
    writer.writeAttribute(QStringLiteral("width"), QString::number(rect.width()));
    writer.writeAttribute(QStringLiteral("height"), QString::number(rect.height()));
    writer.writeAttribute(QStringLiteral("rx"), QString::number(radius));
    writer.writeAttribute(QStringLiteral("ry"), QString::number(radius));
    writer.writeAttribute(QStringLiteral("fill"), QStringLiteral("none"));
    writer.writeAttribute(QStringLiteral("stroke"), QStringLiteral("black"));
    writer.writeEndElement();
}

void SvgExporterPrivate::writeSvgEllipse(const QRectF &rect, bool fill)
{
    writer.writeStartElement(QStringLiteral("ellipse"));
    writer.writeAttribute(QStringLiteral("cx"), QString::number(rect.center().x()));
    writer.writeAttribute(QStringLiteral("cy"), QString::number(rect.center().y()));
    writer.writeAttribute(QStringLiteral("rx"), QString::number(rect.width() / 2.0));
    writer.writeAttribute(QStringLiteral("ry"), QString::number(rect.height() / 2.0));
    writer.writeAttribute(QStringLiteral("fill"), fill ? QStringLiteral("black") : QStringLiteral("none"));
    writer.writeAttribute(QStringLiteral("stroke"), QStringLiteral("black"));
    writer.writeEndElement();
}

void SvgExporterPrivate::writeSvgLine(const QLineF &line)
{
    writer.writeStartElement(QStringLiteral("line"));
    writer.writeAttribute(QStringLiteral("x1"), QString::number(line.x1()));
    writer.writeAttribute(QStringLiteral("y1"), QString::number(line.y1()));
    writer.writeAttribute(QStringLiteral("x2"), QString::number(line.x2()));
    writer.writeAttribute(QStringLiteral("y2"), QString::number(line.y2()));
    writer.writeAttribute(QStringLiteral("stroke"), QStringLiteral("black"));
    writer.writeEndElement();
}

void SvgExporterPrivate::writeSvgPath(const QPainterPath &path, bool fill)
{
    writer.writeStartElement(QStringLiteral("path"));
    QString pathData;
    for (int i = 0; i < path.elementCount(); ++i) {
        auto pathElement = path.elementAt(i);
        switch (pathElement.type) {
        case QPainterPath::MoveToElement:
            pathData += QStringLiteral(" M") + QString::number(pathElement.x) + u',' + QString::number(pathElement.y);
            break;
        case QPainterPath::LineToElement:
            pathData += QStringLiteral(" L") + QString::number(pathElement.x) + u',' + QString::number(pathElement.y);
            break;
        case QPainterPath::CurveToElement:
            pathData += QStringLiteral(" C") + QString::number(pathElement.x) + u',' + QString::number(pathElement.y);
            ++i;
            while (i < path.elementCount()) {
                const auto curveElement = path.elementAt(i);
                if (curveElement.type != QPainterPath::CurveToDataElement) {
                    --i;
                    break;
                }
                pathData += u' ' + QString::number(curveElement.x) + u',' + QString::number(curveElement.y);
                ++i;
            }
            break;
        default:
            break;
        }
    }
    writer.writeAttribute(QStringLiteral("d"), pathData.trimmed());
    writer.writeAttribute(QStringLiteral("fill"), fill ? QStringLiteral("black") : QStringLiteral("none"));
    writer.writeAttribute(QStringLiteral("stroke"), QStringLiteral("black"));
    writer.writeEndElement();
}

void SvgExporterPrivate::writeSvgText(const QString &text, QPointF pos, bool center)
{
    writer.writeStartElement(QStringLiteral("text"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(pos.x()));
    writer.writeAttribute(QStringLiteral("y"), QString::number(pos.y()));
    writer.writeAttribute(QStringLiteral("fill"), QStringLiteral("black"));
    writer.writeAttribute(QStringLiteral("font-size"), QString::number(QGuiApplication::font().pointSizeF()));
    writer.writeAttribute(QStringLiteral("font-family"), QGuiApplication::font().family());
    if (center)
        writer.writeAttribute(QStringLiteral("text-anchor"), QStringLiteral("middle"));
    writer.writeCharacters(text);
    writer.writeEndElement();
}


bool SvgExporterPrivate::writeStateMachine(const StateMachine *machine)
{
    writeSvgRect(machine->boundingRect());
    const QFontMetricsF metrics(QGuiApplication::font());
    const QRectF headerBox(machine->pos().x(), machine->pos().y(),
                           metrics.horizontalAdvance(machine->label()) + 2 * margin(), headerHeight());

    writeSvgRect(headerBox);
    writeSvgText(machine->label(), machine->pos() + QPointF(margin(), headerHeight() / 2.0));
    return writeStateInner(machine);
}

bool SvgExporterPrivate::writeState(State *state)
{
#if !defined(NDEBUG)
    if (auto pseudo = qobject_cast<PseudoState *>(state)) {
        Q_ASSERT(pseudo->kind() == PseudoState::InitialState);
#else
    if (qobject_cast<PseudoState *>(state)) {
#endif
        writeSvgEllipse(state->boundingRect(), true);
    } else if (qobject_cast<FinalState *>(state)) {
        writeSvgEllipse(state->boundingRect(), false);
        const auto dx = state->boundingRect().width() * 0.15;
        const auto dy = state->boundingRect().height() * 0.15;
        writeSvgEllipse(state->boundingRect().adjusted(dx, dy, -dx, -dy), true);
    } else if (auto history = qobject_cast<HistoryState *>(state)) {
        writeSvgEllipse(state->boundingRect(), false);
        if (history->historyType() == HistoryState::DeepHistory) {
            const auto dx = state->boundingRect().width() * 0.1;
            const auto dy = state->boundingRect().height() * 0.1;
            writeSvgEllipse(state->boundingRect().adjusted(dx, dy, -dx, -dy), false);
        }
        writeSvgText(QStringLiteral("H"), history->boundingRect().center(), true);
    } else {
        writeSvgRect(state->boundingRect(), 3.0);
        if (state->isComposite()) {
            writeSvgText(state->label(), state->pos() + QPointF(margin(), headerHeight() / 2.0));
            writeSvgLine(QLineF(state->boundingRect().left(), state->boundingRect().top() + headerHeight(),
                                state->boundingRect().right(), state->boundingRect().top() + headerHeight()));
        } else {
            writeSvgText(state->label(), state->boundingRect().center(), true);
        }
    }
    if (!writeStateInner(state))
        return false;
    return true;
}

bool SvgExporterPrivate::writeStateInner(const State *state)
{
    if (state->transitions().isEmpty() && state->childStates().isEmpty())
        return true;

    writer.writeStartElement(QStringLiteral("g"));
    writer.writeAttribute(QStringLiteral("transform"), QStringLiteral("translate(%1,%2)").arg(state->boundingRect().x()).arg(state->boundingRect().y()));
    const auto stateTransitions = state->transitions();
    for (const Transition *transition : stateTransitions) {
        writeTransition(transition);
    }

    if (state->isExpanded()) {
        const auto childStates = state->childStates();
        for (State *child : childStates) {
            if (auto machine = qobject_cast<StateMachine *>(child)) {
                if (!writeStateMachine(machine))
                    return false;
            } else if (!writeState(child)) {
                return false;
            }
        }
    } else {
        const QPointF p(state->boundingRect().width() / 2.0, state->boundingRect().height() / 2.0 + headerHeight() / 2.0);
        writeSvgText(QStringLiteral("..."), p, true);
    }

    writer.writeEndElement();
    return true;
}

bool SvgExporterPrivate::writeTransition(const Transition *transition)
{
    const auto path = transition->shape().translated(transition->pos());
    writeSvgPath(path);

    QPainterPath arrowHead;
    arrowHead.moveTo(0, 0);
    arrowHead.lineTo(arrowSize(), arrowSize() / 2.0);
    arrowHead.lineTo(arrowSize(), -arrowSize() / 2.0);
    arrowHead.closeSubpath();

    QTransform t;
    t.rotate(180 - path.angleAtPercent(1.0));
    arrowHead = t.map(arrowHead);
    writeSvgPath(arrowHead.translated(path.pointAtPercent(1.0)), true);

    writeSvgText(transition->label(), transition->labelBoundingRect().translated(transition->pos()).topLeft());
    return true;
}


double SvgExporterPrivate::headerHeight()
{
    const QFontMetricsF metrics(QGuiApplication::font());
    return metrics.height() * 2.0;
}

double SvgExporterPrivate::margin()
{
    const QFontMetricsF metrics(QGuiApplication::font());
    return metrics.horizontalAdvance(u'x');
}

double SvgExporterPrivate::arrowSize()
{
    return margin();
}


SvgExporter::SvgExporter(QIODevice *ioDevice)
    : d(new SvgExporterPrivate(this))
{
    d->writer.setDevice(ioDevice);
    d->writer.setAutoFormatting(true);
}

SvgExporter::~SvgExporter()
{
}

bool SvgExporter::exportMachine(StateMachine *machine)
{
    setErrorString({});

    if (!machine) {
        setErrorString(QStringLiteral("Null machine instance passed"));
        return false;
    }

    if (d->writer.hasError()) {
        setErrorString(QStringLiteral("Setting up XML writer failed"));
        return false;
    }

    d->writer.writeStartDocument();
    d->writeSvgIntro(machine->boundingRect().adjusted(-d->margin(), -d->margin(), d->margin(), d->margin()));
    if (!d->writeStateMachine(machine))
        return false;
    d->writer.writeEndElement();
    d->writer.writeEndDocument();
    return !d->writer.hasError();
}
