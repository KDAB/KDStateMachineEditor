/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef KDSME_LAYOUT_GRAPHVIZLAYOUTERBACKEND_P_H
#define KDSME_LAYOUT_GRAPHVIZLAYOUTERBACKEND_P_H

#include <QString>
#include <clocale>

QT_BEGIN_NAMESPACE
class QRectF;
QT_END_NAMESPACE

namespace KDSME {
class LayoutProperties;
class Transition;
class State;
class StateMachineScene;
}

class GraphvizLayouterBackend
{
public:
    enum LayoutMode
    {
        RecursiveMode, ///< Performs a recursive import of all state machine elements, @sa buildState and @sa buildTransitions
        NonRecursiveMode ///< Only a direct import of state machine elements
    };

    GraphvizLayouterBackend();
    ~GraphvizLayouterBackend();

    GraphvizLayouterBackend(const GraphvizLayouterBackend &other) = delete;
    GraphvizLayouterBackend &operator=(const GraphvizLayouterBackend &) = delete;

    /**
     * Defines the layout mode for this instance
     *
     * @note Default is RecursiveMode
     */
    LayoutMode layoutMode() const;
    void setLayoutMode(LayoutMode mode);

    void openLayout(KDSME::State *state, const KDSME::LayoutProperties *properties);
    void closeLayout();

    /**
     * Fill the Graphviz with nodes based on attributes of state @p state
     *
     * @note If RecursiveMode is selected, all descendants of @p state will get imported as well
     */
    void buildState(KDSME::State *state);
    /**
     * Fill Graphviz with edges based on attributes of the transitions of @p state
     *
     * @note If RecursiveMode is selected, transitions of descendants of @p state will get imported as well
     */
    void buildTransitions(const KDSME::State *state);

    void buildTransition(KDSME::Transition *transition);

    void layout();
    void import();

    QRectF boundingRect() const;

    /**
     * Render the current context to file at @p filePath
     *
     * @param filePath the file path
     * @param format 'format' argument to gvRenderFilename (e.g. "png" or "dot")
     *
     * @note Context must be open
     */
    void saveToFile(const QString &filePath, const QString &format = QStringLiteral("png"));

private:
    struct Private;
    Private *const d;
};

/**
 * RAII-based guard for setting the locale to "C" during this object's lifetime
 *
 * Graphviz internally uses atof() and friends to convert strings to numbers, and these are locale-specific.
 * We need to make sure the current locale is "C" so these conversions are done correctly.
 *
 * Also see: https://marc.info/?l=graphviz-interest&m=129408843223794&w=2
 */
class LocaleLocker // clazy:exclude=rule-of-three
{
public:
    inline LocaleLocker()
        : m_oldlocale(qstrdup(setlocale(LC_NUMERIC, nullptr)))
    {
        setlocale(LC_NUMERIC, "C");
    }

    inline ~LocaleLocker()
    {
        setlocale(LC_NUMERIC, m_oldlocale);
        delete[] m_oldlocale;
    }

private:
    const char *m_oldlocale;
};

#endif
