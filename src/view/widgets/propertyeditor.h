/*
  propertyeditor.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef KDSME_WIDGETS_PROPERTYEDITOR_H
#define KDSME_WIDGETS_PROPERTYEDITOR_H

#include "kdsme_view_export.h"

#include <QHash>
#include <QStackedWidget>
#include <QPointer>

class QItemSelectionModel;
class QModelIndex;

namespace Ui {
class StatePropertyEditor;
class TransitionPropertyEditor;
}

namespace KDSME {

class CommandController;
class Element;

class KDSME_VIEW_EXPORT PropertyEditor : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent = 0);
    ~PropertyEditor();

    void setSelectionModel(QItemSelectionModel* selectionModel);
    void setCommandController(CommandController *cmdController);

private slots:
    void updateSimpleProperty();
    void setInitalState(const QString& label);
    void setSourceState(const QString& label);
    void setTargetState(const QString& label);
    void childModeChanged();

    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void loadFromCurrentElement();

private:
    template <typename T> inline T* current() const
    {
        return qobject_cast<T*>(m_currentElement);
    }

    void monitorElement(KDSME::Element* element);

    QItemSelectionModel* m_selectionModel;
    CommandController *m_commandController;
    QPointer<KDSME::Element> m_currentElement;
    Ui::StatePropertyEditor* m_stateWidget;
    Ui::TransitionPropertyEditor* m_transitionWidget;
    int m_noWidgetIndex, m_stateWidgetIndex, m_transitionWidgetIndex;

    QHash<QObject*, QString> m_widgetToPropertyMap;
};

} // namespace KDSME

#endif // PROPERTYEDITOR_H
