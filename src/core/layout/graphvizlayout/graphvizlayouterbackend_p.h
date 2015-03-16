/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_LAYOUT_GRAPHVIZLAYOUTERBACKEND_H
#define KDSME_LAYOUT_GRAPHVIZLAYOUTERBACKEND_H

#include <QString>
#include <clocale>

class QRectF;

namespace KDSME {
class Transition;
class State;
class StateMachineScene;
}

class GraphvizLayouterBackend
{
public:
    enum LayoutMode {
        RecursiveMode, ///< Performs a recursive import of all state machine elements, @sa buildState and @sa buildTransitions
        NonRecursiveMode ///< Only a direct import of state machine elements
    };

    GraphvizLayouterBackend();
    ~GraphvizLayouterBackend();

    /**
     * Defines the layout mode for this instance
     *
     * @note Default is RecursiveMode
     */
    LayoutMode layoutMode() const;
    void setLayoutMode(LayoutMode mode);

    void openContext();
    void closeContext();

    /**
     * Fill the Graphviz with nodes based on attributes of state @p state
     *
     * @note If RecursiveMode is selected, all descendants of @p state will get imported as well
     */
    void buildState(KDSME::State* state);
    /**
     * Fill Graphviz with edges based on attributes of the transitions of @p state
     *
     * @note If RecursiveMode is selected, transitions of descendants of @p state will get imported as well
     */
    void buildTransitions(KDSME::State* state);

    void buildTransition(KDSME::Transition* transition);

    void layout();
    void import();

    QRectF boundingRect() const;

    /**
     * Render the current context to file at @p filePath
     *
     * @param format 'format' argument to gvRenderFilename (e.g. "png" or "dot")
     *
     * @note Context must be open
     */
    void saveToFile(const QString& filePath, const QString& format = "png");

private:
    struct Private;
    Private* const d;
};

/**
 * RAII-based guard for setting the locale to "C" during this object's lifetime
 *
 * Graphviz internally uses atof() and friends to convert strings to numbers, and these are locale-specific.
 * We need to make sure the current locale is "C" so these conversions are done correctly.
 *
 * Also see: http://lists.research.att.com/pipermail/graphviz-interest/2011q1/006316.html
 */
class LocaleLocker
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
    const char* m_oldlocale;
};

#endif
